// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "InterruptVectorTable.h"
#include "Exceptions.h"
#include "Process/ProcessManager.h"
#include <ConsoleIO.h>
#include <PhysicalAddress.h>

uint64_t counter = 0;

alignas(8) DescriptorTablePointer g_idt_pointer{};
alignas(8) InterruptDescriptor g_interrupts[256];

INTERRUPT_HANDLER(not_implemented);
void not_implemented_handler(StackFrame frame) {
    printf("\u001b[31mNot implemented!\u001b[0m\n");
    printf("    rip = %X\n", frame.rip);
    printf("     cs = %X\n", frame.cs);
    printf(" rflags = %X\n", frame.rflags);
    printf("    rsp = %X\n", frame.rsp);
    printf("     ss = %X\n", frame.ss);
}

INTERRUPT_HANDLER(timer)
void timer_handler(StackFrame frame) {
    if (counter++ % 100 == 0) {
        printf("\u001b[42;97m Timer \u001b[0m rsp = %X, counter = %d\n", frame.rsp, counter++);
    }

    auto eoi_register = PhysicalAddress(0xFEE00000 + 0xb0).as_ptr<uint32_t>();
    *eoi_register = 0;
}

struct PACKED BacktraceFrame {
    BacktraceFrame *rbp;
    uint64_t rip;
};

INTERRUPT_HANDLER(schedule)
void schedule_handler(StackFrame frame) {
    auto &process_manager = Kernel::ProcessManager::get_instance();

    // FIXME: hack for now, let's just see if we can jump into user code
    auto next_process = process_manager.next_process();

    auto next_rsp = next_process->get_rsp().as_address();
    asm volatile("mov %0, %%rsp" ::"b"(next_rsp)
                 : "memory");
    POP_REGISTERS();
    asm volatile("iretq");
}

void InterruptVectorTable::initialise() {
    for (int i = 0; i < 255; i++) {
        set_interrupt_gate(i, PrivilegeLevel::Kernel, &not_implemented_asm_wrapper);
    }
    configure_exceptions(*this);
    set_interrupt_gate(32, PrivilegeLevel::User, &timer_asm_wrapper);
    set_interrupt_gate(34, PrivilegeLevel::Kernel, &schedule_asm_wrapper);

    g_idt_pointer.address = reinterpret_cast<uint64_t>(&g_interrupts);
    g_idt_pointer.limit = sizeof(InterruptDescriptor) * 256;
    asm volatile("lidt g_idt_pointer");
}

void InterruptVectorTable::set_interrupt_gate(uint8_t id, PrivilegeLevel dpl, void (*handler)()) {
    auto descriptor_privilege_level = dpl == PrivilegeLevel::User ? 3 : 0;
    auto address = reinterpret_cast<uint64_t>(handler);
    g_interrupts[id].offset = address & 0xffff;
    g_interrupts[id].offset_2 = (address >> 16) & 0xffff;
    g_interrupts[id].offset_3 = address >> 32;
    g_interrupts[id].segment_selector = 0x08;
    g_interrupts[id].type = 0x0f;
    g_interrupts[id].descriptor_privilege_level = descriptor_privilege_level;
    g_interrupts[id].present = 1;
}
