// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "LibStd/Result.h"
#include <Heap/SlabAllocator.h>
#include "LibStd/Vector.h"
#include <ConsoleIO.h>

class Device {
public:
    Device(const u16 vendor_id, const u16 device_id, const u32 bar0, const u32 bar1, const u32 bar2, const u32 bar3) : m_vendor_id(vendor_id)
                                                                    , m_device_id(device_id)
                                                                    , m_bar0(bar0)
                                                                    , m_bar1(bar1)
                                                                    , m_bar2(bar2)
                                                                    , m_bar3(bar3) {}

    void *operator new(size_t size) noexcept {
        auto &slab_allocator = Kernel::SlabAllocator::get_instance();
        auto allocator_or_error = slab_allocator.get_or_create_slab(sizeof(Device));

        if(allocator_or_error.is_error()) {
            println("FATAL: Could not get allocator for size {}", sizeof(Device));
            return nullptr;
        }

        const auto allocator = allocator_or_error.get().as_ptr();
        const auto ptr_or_error = allocator->allocate<Device>();
        if(ptr_or_error.is_error()) {
            println("FATAL: Failed to allocate Device in Slab");
            return nullptr;
        }

        return ptr_or_error.get();
    }

    void operator delete(void *ptr) {
        auto &slab_allocator = Kernel::SlabAllocator::get_instance();
        const auto allocator_or_error = slab_allocator.get_or_create_slab(sizeof(Device));

        if(!allocator_or_error.is_error()) {
            if(const auto allocator = allocator_or_error.get().as_ptr(); allocator->free(VirtualAddress{ptr}).is_error()) {
                println("PCI: failed to free device address = {}", ptr);
            }
        }
    }

    [[nodiscard]] u16 vendor_id() const { return m_vendor_id; }
    [[nodiscard]] u16 device_id() const { return m_device_id; }
    [[nodiscard]] u32 bar0() const { return m_bar0; }
    [[nodiscard]] u32 bar1() const { return m_bar1; }
    [[nodiscard]] u32 bar2() const { return m_bar2; }
    [[nodiscard]] u32 bar3() const { return m_bar3; }

private:
    u16 m_vendor_id{0};
    u16 m_device_id{0};
    u32 m_bar0{0};
    u32 m_bar1{0};
    u32 m_bar2{0};
    u32 m_bar3{0};
    u32 m_bar4{0};
    u32 m_bar5{0};
    Std::Vector<Device *> m_children;
};

class PCI {
public:
    Std::Result<void> initialise();
    Std::Optional<Device *> find_device(u16 vendor_id, u16 device_id);

private:
    Std::Result<void> initialise_legacy();
    Std::Vector<Device *> m_devices;

    Std::Result<void> read_device_function(u8 bus, u8 device, u8 function);
    u16 config_read_word(u8 bus, u8 slot, u8 function, u8 offset);
    static u32 config_read_dword(u8 bus, u8 slot, u8 function, u8 offset);

    union Address {
         struct {
            u32 offset : 8;
            u32 function : 3;
            u32 device_number : 5;
            u32 bus_number : 8;
            u32 reserved : 7;
            u32 enable : 1;
        };
        u32 value;
    };
};
