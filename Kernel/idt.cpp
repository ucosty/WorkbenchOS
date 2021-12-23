// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <Descriptors.h>
#include <Types.h>

extern "C" [[noreturn]] void exception() {
    asm volatile("mov $0xdeadc0de, %rax\n"
                 "pop %rbp\n"
                 "iretq");
    while (true) {}
}

alignas(8) DescriptorTablePointer idt_pointer{};
alignas(8) InterruptDescriptor interrupts[20];

void configure_interrupts() {
    for (auto &interrupt: interrupts) {
        interrupt.offset = reinterpret_cast<uint64_t>(&exception) & 0xffff;
        interrupt.offset_2 = (reinterpret_cast<uint64_t>(&exception) >> 16) & 0xffff;
        interrupt.offset_3 = reinterpret_cast<uint64_t>(&exception) >> 32;
        interrupt.segment_selector = 0x08;
        interrupt.type = 0x0f;
        interrupt.descriptor_privilege_level = 0;
        interrupt.present = 1;
    }
    idt_pointer.address = reinterpret_cast<uint64_t>(&interrupts);
    idt_pointer.limit = sizeof(InterruptDescriptor) * 20;
    asm volatile("lidt idt_pointer\n"
                 "int $0x01");
}