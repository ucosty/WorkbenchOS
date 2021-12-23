// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Exceptions.h"
#include <ConsoleIO.h>
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

void set_exception_handler(int id, uint64_t handler) {
    interrupts[id].offset = handler & 0xffff;
    interrupts[id].offset_2 = (handler >> 16) & 0xffff;
    interrupts[id].offset_3 = handler >> 32;
    interrupts[id].segment_selector = 0x08;
    interrupts[id].type = 0x0f;
    interrupts[id].descriptor_privilege_level = 0;
    interrupts[id].present = 1;
}

void NAKED divide_by_zero_exception() {
    asm volatile("mov $0xdeadc0de, %rax\n"
                 "mov $0x80000000, %rbx\n"
                 "hlt");
}

struct StackFrame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct StackFrameErrorCode {
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

#define EXCEPTION_HANDLER(name) \
    extern "C" void name##_handler(StackFrame); \
    void NAKED name##_asm_wrapper() {           \
        asm volatile("call " #name "_handler\n" \
                     "hlt");                    \
    }

#define EXCEPTION_HANDLER_WITH_CODE(name) \
    extern "C" void name##_handler(StackFrameErrorCode); \
    void NAKED name##_asm_wrapper() {           \
        asm volatile("call " #name "_handler\n" \
                     "hlt");                    \
    }

EXCEPTION_HANDLER(divide_by_zero_exception);
void divide_by_zero_exception_handler(StackFrame frame) {
}

EXCEPTION_HANDLER(debug_exception);
void debug_exception_handler(StackFrame frame) {
    printf("Debug Exception!\n");
    printf("   rip = %X\n", frame.rip);
    printf("    cs = %X\n", frame.cs);
    printf("rflags = %X\n", frame.rflags);
    printf("   rsp = %X\n", frame.rsp);
    printf("    ss = %X\n", frame.ss);
}

EXCEPTION_HANDLER(nmi_interrupt);
void nmi_interrupt_handler(StackFrame frame) {
    printf("NMI Interrupt Exception!\n");
}

EXCEPTION_HANDLER(breakpoint_exception);
void breakpoint_exception_handler(StackFrame frame) {
    printf("Breakpoint Exception!\n");
}

EXCEPTION_HANDLER(overflow_exception);
void overflow_exception_handler(StackFrame frame) {
    printf("Overflow Exception!\n");
}

EXCEPTION_HANDLER(bound_range_exceeded_exception);
void bound_range_exceeded_exception_handler(StackFrame frame) {
    printf("Bound Range Exceeded Exception!\n");
}

EXCEPTION_HANDLER(invalid_opcode_exception);
void invalid_opcode_exception_handler(StackFrame frame) {
    printf("Invalid Opcode Exception!\n");
}

EXCEPTION_HANDLER(device_not_available_exception);
void device_not_available_exception_handler(StackFrame frame) {
    printf("Device Not Available Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(double_fault_exception);
void double_fault_exception_handler(StackFrameErrorCode frame) {
    printf("Double Fault Exception!\n");
}

EXCEPTION_HANDLER(coprocessor_segment_overrun_exception);
void coprocessor_segment_overrun_exception_handler(StackFrame frame) {
    printf("Co-Processor Segment Overrun Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(invalid_tss_exception);
void invalid_tss_exception_handler(StackFrameErrorCode frame) {
    printf("Invalid TSS Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(segment_not_present_exception);
void segment_not_present_exception_handler(StackFrameErrorCode frame) {
    printf("Segment Not Present Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(stack_fault_exception);
void stack_fault_exception_handler(StackFrameErrorCode frame) {
    printf("Stack Fault Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(general_protection_fault_exception);
void general_protection_fault_exception_handler(StackFrameErrorCode frame) {
    printf("General Protection Fault Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(page_fault_exception);
void page_fault_exception_handler(StackFrameErrorCode frame) {
    printf("Page Fault Exception!\n");
}

EXCEPTION_HANDLER(floating_point_error_exception);
void floating_point_error_exception_handler(StackFrame frame) {
    printf("Floating Point Exception!\n");
}

EXCEPTION_HANDLER_WITH_CODE(alignment_check_exception);
void alignment_check_exception_handler(StackFrameErrorCode frame) {
    printf("Alignment Check Exception!\n");
}

EXCEPTION_HANDLER(machine_check_exception);
void machine_check_exception_handler(StackFrame frame) {
    printf("Machine Check Exception!\n");
}

EXCEPTION_HANDLER(simd_floating_point_exception);
void simd_floating_point_exception_handler(StackFrame frame) {
    printf("SIMD Floating Point Exception!\n");
}

EXCEPTION_HANDLER(virtualisation_exception);
void virtualisation_exception_handler(StackFrame frame) {
    printf("Virtualisation Exception!\n");
}

void configure_interrupts() {
    set_exception_handler(0, reinterpret_cast<uint64_t>(&divide_by_zero_exception_asm_wrapper));
    set_exception_handler(1, reinterpret_cast<uint64_t>(&debug_exception_asm_wrapper));
    set_exception_handler(2, reinterpret_cast<uint64_t>(&nmi_interrupt_asm_wrapper));
    set_exception_handler(3, reinterpret_cast<uint64_t>(&breakpoint_exception_asm_wrapper));
    set_exception_handler(4, reinterpret_cast<uint64_t>(&overflow_exception_asm_wrapper));
    set_exception_handler(5, reinterpret_cast<uint64_t>(&bound_range_exceeded_exception_asm_wrapper));
    set_exception_handler(6, reinterpret_cast<uint64_t>(&invalid_opcode_exception_asm_wrapper));
    set_exception_handler(7, reinterpret_cast<uint64_t>(&device_not_available_exception_asm_wrapper));
    set_exception_handler(8, reinterpret_cast<uint64_t>(&double_fault_exception_asm_wrapper));
    set_exception_handler(9, reinterpret_cast<uint64_t>(&coprocessor_segment_overrun_exception_asm_wrapper));
    set_exception_handler(10, reinterpret_cast<uint64_t>(&invalid_tss_exception_asm_wrapper));
    set_exception_handler(11, reinterpret_cast<uint64_t>(&segment_not_present_exception_asm_wrapper));
    set_exception_handler(12, reinterpret_cast<uint64_t>(&stack_fault_exception_asm_wrapper));
    set_exception_handler(13, reinterpret_cast<uint64_t>(&general_protection_fault_exception_asm_wrapper));
    set_exception_handler(14, reinterpret_cast<uint64_t>(&page_fault_exception_asm_wrapper));
    set_exception_handler(16, reinterpret_cast<uint64_t>(&floating_point_error_exception_asm_wrapper));
    set_exception_handler(17, reinterpret_cast<uint64_t>(&alignment_check_exception_asm_wrapper));
    set_exception_handler(18, reinterpret_cast<uint64_t>(&machine_check_exception_asm_wrapper));
    set_exception_handler(19, reinterpret_cast<uint64_t>(&simd_floating_point_exception_asm_wrapper));
    set_exception_handler(20, reinterpret_cast<uint64_t>(&virtualisation_exception_asm_wrapper));

    idt_pointer.address = reinterpret_cast<uint64_t>(&interrupts);
    idt_pointer.limit = sizeof(InterruptDescriptor) * 20;
    asm volatile("lidt idt_pointer");
}
