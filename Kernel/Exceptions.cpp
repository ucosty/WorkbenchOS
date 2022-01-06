// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "InterruptVectorTable.h"
#include <ConsoleIO.h>
#include <Types.h>

extern uint64_t counter;

EXCEPTION_HANDLER(divide_by_zero_exception);
void divide_by_zero_exception_handler(StackFrame frame) {
    printf("Divide by zero!\n");
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
    printf("\u001b[31mGeneral Protection Fault Exception!\u001b[0m\n");
    printf("    rip = %X\n", frame.rip);
    printf("     cs = %X\n", frame.cs);
    printf(" rflags = %X\n", frame.rflags);
    printf("    rsp = %X\n", frame.rsp);
    printf("     ss = %X\n", frame.ss);
    printf("  error = %X\n", frame.error);
}

struct PACKED BacktraceFrame {
    BacktraceFrame *rbp;
    uint64_t rip;
};
static_assert(sizeof(BacktraceFrame) == 16);

EXCEPTION_HANDLER_WITH_CODE(page_fault_exception);
void page_fault_exception_handler(StackFrameErrorCode frame) {
    uint64_t address = 0;
    asm volatile("movq %%cr2, %%rax"
                 : "=a"(address));
    printf("\u001b[31mPage Fault Exception!\u001b[0m\n");
    printf("    rip = %X\n", frame.rip);
    printf("     cs = %X\n", frame.cs);
    printf(" rflags = %X\n", frame.rflags);
    printf("    rsp = %X\n", frame.rsp);
    printf("     ss = %X\n", frame.ss);
    printf("  error = %X\n", frame.error);
    printf("address = %X\n", address);
    printf("counter = %X\n", counter);

    printf("\nBacktrace:\n");
    volatile BacktraceFrame *stack_frame;
    asm("movq %%rbp, %0"
        : "=r"(stack_frame));

    for (int i = 0; i < 10; i++) {
        printf("%i: rip = %X\n", i, stack_frame->rip);

        if(stack_frame->rbp == nullptr || stack_frame->rbp == stack_frame)
            break;

        stack_frame = stack_frame->rbp;
    }
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

void configure_exceptions(InterruptVectorTable &ivt) {
    InterruptVectorTable::set_interrupt_gate(0, PrivilegeLevel::Kernel, &divide_by_zero_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(0, PrivilegeLevel::Kernel, &divide_by_zero_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(1, PrivilegeLevel::Kernel, &debug_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(2, PrivilegeLevel::Kernel, &nmi_interrupt_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(3, PrivilegeLevel::Kernel, &breakpoint_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(4, PrivilegeLevel::Kernel, &overflow_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(5, PrivilegeLevel::Kernel, &bound_range_exceeded_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(6, PrivilegeLevel::Kernel, &invalid_opcode_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(7, PrivilegeLevel::Kernel, &device_not_available_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(8, PrivilegeLevel::Kernel, &double_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(9, PrivilegeLevel::Kernel, &coprocessor_segment_overrun_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(10, PrivilegeLevel::Kernel, &invalid_tss_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(11, PrivilegeLevel::Kernel, &segment_not_present_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(12, PrivilegeLevel::Kernel, &stack_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(13, PrivilegeLevel::Kernel, &general_protection_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(14, PrivilegeLevel::Kernel, &page_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(16, PrivilegeLevel::Kernel, &floating_point_error_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(17, PrivilegeLevel::Kernel, &alignment_check_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(18, PrivilegeLevel::Kernel, &machine_check_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(19, PrivilegeLevel::Kernel, &simd_floating_point_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(20, PrivilegeLevel::Kernel, &virtualisation_exception_asm_wrapper);
}
