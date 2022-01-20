// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Result.h>
#include <Heap/SlabAllocator.h>
#include <Vector.h>
#include <ConsoleIO.h>

class Device {
public:
    Device(uint16_t vendor_id, uint16_t device_id) : m_vendor_id(vendor_id), m_device_id(device_id) {}

    void *operator new(size_t size) noexcept {
        auto &slab_allocator = Kernel::SlabAllocator::get_instance();
        auto allocator_or_error = slab_allocator.get_or_create_slab(sizeof(Device));

        if(allocator_or_error.is_error()) {
            printf("FATAL: Could not get allocator for size %d\n", sizeof(Device));
            return nullptr;
        }

        auto allocator = allocator_or_error.get().as_ptr();

        auto ptr_or_error = allocator->allocate<Device>();
        if(ptr_or_error.is_error()) {
            printf("FATAL: Failed to allocate Device in Slab\n");
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
                printf("PCI: failed to free device address = %X\n", ptr);
            }
        }
    }

private:
    uint16_t m_vendor_id;
    uint16_t m_device_id;
    Lib::Vector<Device *> m_children;
};

class PCI {
public:
    Result<void> initialise();

private:
    Result<void> initialise_legacy();
    Lib::Vector<Device *> m_devices;

    Result<void> read_device_function(uint8_t bus, uint8_t device, uint8_t function);
    uint16_t config_read_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
    uint32_t config_read_dword(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);

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
