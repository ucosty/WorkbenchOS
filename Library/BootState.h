// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <EFI/MemoryMap.h>
#include <PhysicalAddress.h>
#include "LibStd/Types.h"

struct PACKED Framebuffer {
    size_t size;
    uint64_t width, height;
};

struct PACKED VirtualMapping {
    uint64_t physical_base;
    uint64_t virtual_base;
    size_t size;
};

struct PACKED ProgramHeaderFlags {
    u64 address{0};
    u64 size{0};
    u8 flags{0};
};

struct PACKED KernelAddressSpace {
    uint64_t kernel_page_directory_virtual_address;
    uint64_t base;
    size_t size;

    VirtualMapping kernel;
    VirtualMapping boot_state;
    VirtualMapping initial_pages;
    VirtualMapping memory_map;
    VirtualMapping framebuffer;
    VirtualMapping stack;
    VirtualMapping frame_allocator;

    // Kernel Program header flags
    uint8_t program_header_count{0};
    ProgramHeaderFlags program_header_flags[8];
};

struct PACKED Ramdisk {
    PhysicalAddress address;
    size_t size;
};

struct PACKED BootState {
    uint64_t physical_memory_size;
    uint64_t acpi_root_table_address;
    Framebuffer framebuffer;
    EFI::MemoryMap memory_map;
    KernelAddressSpace kernel_address_space;
    Ramdisk ramdisk;
};
