// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "PIC.h"
#include <BootState.h>
#include <Descriptors.h>
#include <Processor.h>

static constexpr size_t gdt_descriptors = 7;
alignas(8) DescriptorTablePointer gdt_pointer{};
alignas(8) u64 segments[gdt_descriptors] = {
    // Null segment
    SegmentDescriptor{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}.descriptor(),
    // Kernel Code Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_EXECUTE, 1, 0, 1, 0, 0, 1, 0, 0, 0}.descriptor(),
    // Kernel Data Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_READ_WRITE, 1, 0, 1, 0, 0, 0, 0, 0, 0}.descriptor(),
    // User Code Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_EXECUTE, 1, 3, 1, 0, 0, 1, 0, 0, 0}.descriptor(),
    // User Data Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_READ_WRITE, 1, 3, 1, 0, 0, 0, 0, 0, 0}.descriptor(),
    // TSS 0
    0, 0};

alignas(16) TSS tss0;

typedef void (*constructor_function)();
extern constructor_function __init_array_start[];
extern constructor_function __init_array_end[];

extern "C" [[noreturn]] void kernel_stage2(const BootState &boot_state) {
    for (auto init = __init_array_start; init < __init_array_end; init++) {
        (*init)();
    }

    PIC::disable();

    // Platform specific init code goes here
    // Generic kernel init goes here
}

extern "C" [[noreturn]] EFICALL void kernel_main(u64 boot_state_address) {
    gdt_pointer.address = reinterpret_cast<u64>(&segments);
    gdt_pointer.limit = sizeof(SegmentDescriptor) * gdt_descriptors;

    auto tss0_address = reinterpret_cast<u64>(&tss0);
    TSSDescriptor tss_descriptor_0{
        .segment_limit = sizeof(TSS) - 1,
        .base_low = tss0_address & 0xffff,
        .base_low_middle = (tss0_address >> 16) & 0xff,
        .segment_type = 9,
        .privilege_level = 0,
        .present = 1,
        .granularity = 0,
        .base_high_middle = (tss0_address >> 24) & 0xff,
        .base_high = (u32) (tss0_address >> 32),
    };

    segments[5] = tss_descriptor_0.low;
    segments[6] = tss_descriptor_0.high;
    asm volatile("lgdt gdt_pointer\n"
                 "mov %%rax, %%rdi\n"// Boot state block
                 "mov $0x10, %%rbx\n"
                 "mov %%rbx, %%ss\n"
                 "mov %%rbx, %%ds\n"
                 "mov %%rbx, %%es\n"
                 "mov %%rbx, %%fs\n"
                 "mov %%rbx, %%gs\n"
                 "movq $0x0, %%rbp\n"
                 "pushq $0x08\n"         // Push CS
                 "pushq $kernel_stage2\n"// Push RIP
                 "lretq\n"
                 : /* no output */
                 : "a"(boot_state_address));
    Kernel::Processor::halt();
}
