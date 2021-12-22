// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

struct Framebuffer {
    uint64_t base_address;
    size_t size;
    uint64_t width, height;
};

struct VirtualMapping {
    uint64_t physical_base;
    uint64_t virtual_base;
    size_t size;
};

struct KernelAddressSpace {
    uint64_t base;
    size_t size;

    VirtualMapping kernel;
    VirtualMapping boot_state;
    VirtualMapping initial_pages;
    VirtualMapping memory_map;
    VirtualMapping framebuffer;
    VirtualMapping stack;
};

struct BootState {
    Framebuffer framebuffer;
    KernelAddressSpace kernel_address_space;
};
