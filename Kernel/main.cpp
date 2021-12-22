// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <BootState.h>
#include <GDT.h>
#include <LinearFramebuffer.h>
#include <Types.h>

alignas(8) DescriptorTablePointer gdt_pointer{};
alignas(8) SegmentDescriptor segments[5] = {
    // Null segment
    {},
    // Kernel Code Segment
    {
        .segment_type = SEGMENT_EXECUTE,
        .descriptor_type = DESCRIPTOR_CODE_DATA,
        .present = 1,
        .code_segment_64bit = 1,
    },
    // Kernel Data Segment
    {
        .segment_type = SEGMENT_READ_WRITE,
        .descriptor_type = DESCRIPTOR_CODE_DATA,
        .present = 1,
        .code_segment_64bit = 0,
    },
    // User Code Segment
    {
        .segment_type = SEGMENT_EXECUTE,
        .descriptor_type = DESCRIPTOR_CODE_DATA,
        .privilege_level = 3,
        .present = 1,
        .code_segment_64bit = 1,
    },
    // User Data Segment
    {
        .segment_type = SEGMENT_READ_WRITE,
        .descriptor_type = DESCRIPTOR_CODE_DATA,
        .privilege_level = 3,
        .present = 1,
        .code_segment_64bit = 0,
    },
};

extern "C" [[noreturn]] __attribute__((ms_abi)) void kernel_main(BootState *boot_state) {
    auto framebuffer = LinearFramebuffer(boot_state->kernel_address_space.framebuffer.virtual_base, 1280, 1024);

    // TODO: Relocate the stack -- done
    // TODO: Set up new GDT and jump into new kernel code segment -- done
    // TODO: Set up global page tables for kernel and whole-memory access
    // TODO: Set up IDT and basic exception handlers
    // TODO: Early stage boot console
    gdt_pointer.address = reinterpret_cast<uint64_t>(&segments);
    gdt_pointer.limit = 5;
    asm volatile("lgdt (gdt_pointer)");

    framebuffer.rect(50, 50, 100, 100, 0x4455aa, true);

    for (;;) {}
}
