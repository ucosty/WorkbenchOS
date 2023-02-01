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
    Device(uint16_t vendor_id, uint16_t device_id, uint32_t bar0, uint32_t bar1, uint32_t bar2, uint32_t bar3) : m_vendor_id(vendor_id)
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

        auto allocator = allocator_or_error.get().as_ptr();

        auto ptr_or_error = allocator->allocate<Device>();
        if(ptr_or_error.is_error()) {
            println("FATAL: Failed to allocate Device in Slab");
            return nullptr;
        }

        return ptr_or_error.get();
    }

    void operator delete(void *ptr) {
        auto &slab_allocator = Kernel::SlabAllocator::get_instance();
        auto allocator_or_error = slab_allocator.get_or_create_slab(sizeof(Device));

        if(!allocator_or_error.is_error()) {
            auto allocator = allocator_or_error.get().as_ptr();
            if(allocator->free(VirtualAddress{ptr}).is_error()) {
                println("PCI: failed to free device address = {}", ptr);
            }
        }
    }

    [[nodiscard]] uint16_t vendor_id() const { return m_vendor_id; }
    [[nodiscard]] uint16_t device_id() const { return m_device_id; }
    [[nodiscard]] uint32_t bar0() const { return m_bar0; }
    [[nodiscard]] uint32_t bar1() const { return m_bar1; }
    [[nodiscard]] uint32_t bar2() const { return m_bar2; }
    [[nodiscard]] uint32_t bar3() const { return m_bar3; }

private:
    uint16_t m_vendor_id;
    uint16_t m_device_id;
    uint32_t m_bar0;
    uint32_t m_bar1;
    uint32_t m_bar2;
    uint32_t m_bar3;
    uint32_t m_bar4;
    uint32_t m_bar5;
    Std::Vector<Device *> m_children;
};

class PCI {
public:
    Std::Result<void> initialise();
    Std::Optional<Device *> find_device(uint16_t vendor_id, uint16_t device_id);

private:
    Std::Result<void> initialise_legacy();
    Std::Vector<Device *> m_devices;

    Std::Result<void> read_device_function(uint8_t bus, uint8_t device, uint8_t function);
    uint16_t config_read_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
    static uint32_t config_read_dword(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);

    union Address {
         struct {
            uint32_t offset : 8;
            uint32_t function : 3;
            uint32_t device_number : 5;
            uint32_t bus_number : 8;
            uint32_t reserved : 7;
            uint32_t enable : 1;
        };
        uint32_t value;
    };
};
