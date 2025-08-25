// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <ConsoleIO.h>
#include <Exceptions.h>
#include <PhysicalAddress.h>
#include <Process/ProcessManager.h>
#include <Devices/PS2Mouse.h>
#include <Devices/PS2Keyboard.h>
u64 counter = 0;

alignas(8) DescriptorTablePointer g_idt_pointer{};
alignas(8) InterruptDescriptor g_interrupts[256];

PS2Mouse g_mouse;
PS2Keyboard g_keyboard;

INTERRUPT_HANDLER(0, not_implemented);
void not_implemented_handler(StackFrame *frame) {
    println("\u001b[31mNot implemented!\u001b[0m");
    println("    rip = {}", frame->rip);
    println("     cs = {}", frame->cs);
    println(" rflags = {}", frame->rflags);
    println("    rsp = {}", frame->rsp);
    println("     ss = {}", frame->ss);
}

INTERRUPT_HANDLER(32, timer)
void timer_handler(StackFrame *frame) {
    //    if (counter++ % 100 == 0) {
    println("\u001b[42;97m Timer \u001b[0m rsp = {}, counter = {}", frame->rsp, counter++);
    //    }

    const auto eoi_register = PhysicalAddress(0xFEE00000 + 0xb0).as_ptr<u32>();
    *eoi_register = 0;
}

INTERRUPT_HANDLER(33, ps2_keyboard)
void ps2_keyboard_handler(StackFrame *frame) {
    g_keyboard.interrupt_handler();
    const auto eoi_register = PhysicalAddress(0xFEE00000 + 0xb0).as_ptr<u32>();
    *eoi_register = 0;
}

INTERRUPT_HANDLER(34, ps2_mouse)
void ps2_mouse_handler(StackFrame *frame) {
    g_mouse.interrupt_handler();
    const auto eoi_register = PhysicalAddress(0xFEE00000 + 0xb0).as_ptr<u32>();
    *eoi_register = 0;
}

struct PACKED BacktraceFrame {
    BacktraceFrame *rbp;
    u64 rip;
};

INTERRUPT_HANDLER(35, schedule)
void schedule_handler(StackFrame *frame) {
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
    set_interrupt_gate(33, PrivilegeLevel::Kernel, &ps2_keyboard_asm_wrapper);
    set_interrupt_gate(34, PrivilegeLevel::Kernel, &ps2_mouse_asm_wrapper);

    g_idt_pointer.address = reinterpret_cast<u64>(&g_interrupts);
    g_idt_pointer.limit = sizeof(InterruptDescriptor) * 256;
    asm volatile("lidt g_idt_pointer");
}

void InterruptVectorTable::set_interrupt_gate(u8 id, PrivilegeLevel dpl, void (*handler)()) {
    const auto descriptor_privilege_level = dpl == PrivilegeLevel::User ? 3 : 0;
    const auto address = reinterpret_cast<u64>(handler);
    g_interrupts[id].offset = address & 0xffff;
    g_interrupts[id].offset_2 = (address >> 16) & 0xffff;
    g_interrupts[id].offset_3 = address >> 32;
    g_interrupts[id].segment_selector = 0x08;
    g_interrupts[id].type = 0x0f;
    g_interrupts[id].descriptor_privilege_level = descriptor_privilege_level;
    g_interrupts[id].present = 1;
}
