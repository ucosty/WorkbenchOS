// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <InterruptVectorTable.h>
#include <LibStd/Types.h>
#include <UnbufferedConsole.h>

EXCEPTION_HANDLER(0, divide_by_zero_exception);
void divide_by_zero_exception_handler(StackFrame *frame) {
    println("Divide by zero!");
}

EXCEPTION_HANDLER(1, debug_exception);
void debug_exception_handler(StackFrame *frame) {
    println("Debug Exception!");
    println("   rip = {:#x}", frame->rip);
    println("    cs = {:#x}", frame->cs);
    println("rflags = {:#x}", frame->rflags);
    println("   rsp = {:#x}", frame->rsp);
    println("    ss = {:#x}", frame->ss);
}

EXCEPTION_HANDLER(2, nmi_interrupt);
void nmi_interrupt_handler(StackFrame *frame) {
    println("NMI Interrupt Exception!");
}

EXCEPTION_HANDLER(3, breakpoint_exception);
void breakpoint_exception_handler(StackFrame *frame) {
    println("Breakpoint Exception!");
}

EXCEPTION_HANDLER(4, overflow_exception);
void overflow_exception_handler(StackFrame *frame) {
    println("Overflow Exception!");
}

EXCEPTION_HANDLER(5, bound_range_exceeded_exception);
void bound_range_exceeded_exception_handler(StackFrame *frame) {
    println("Bound Range Exceeded Exception!");
}

EXCEPTION_HANDLER(6, invalid_opcode_exception);
void invalid_opcode_exception_handler(StackFrame *frame) {
    println("Invalid Opcode Exception!");
    println("    rip = {:#x}", frame->rip);
    println("     cs = {:#x}", frame->cs);
    println(" rflags = {:#x}", frame->rflags);
    println("    rbp = {:#x}", frame->rbp);
    println("    rsp = {:#x}", frame->rsp);
    println("     ss = {:#x}", frame->ss);
}

EXCEPTION_HANDLER(7, device_not_available_exception);
void device_not_available_exception_handler(StackFrame *frame) {
    println("Device Not Available Exception!");
}

EXCEPTION_HANDLER_WITH_CODE(8, double_fault_exception);
void double_fault_exception_handler(StackFrameErrorCode *frame) {
    println("Double Fault Exception!");
}

EXCEPTION_HANDLER(9, coprocessor_segment_overrun_exception);
void coprocessor_segment_overrun_exception_handler(StackFrame *frame) {
    println("Co-Processor Segment Overrun Exception!");
}

EXCEPTION_HANDLER_WITH_CODE(10, invalid_tss_exception);
void invalid_tss_exception_handler(StackFrameErrorCode *frame) {
    println("Invalid TSS Exception!");
}

EXCEPTION_HANDLER_WITH_CODE(11, segment_not_present_exception);
void segment_not_present_exception_handler(StackFrameErrorCode *frame) {
    println("Segment Not Present Exception!");
}

EXCEPTION_HANDLER_WITH_CODE(12, stack_fault_exception);
void stack_fault_exception_handler(StackFrameErrorCode *frame) {
    println("Stack Fault Exception!");
}

EXCEPTION_HANDLER_WITH_CODE(13, general_protection_fault_exception);
void general_protection_fault_exception_handler(StackFrameErrorCode *frame) {
    println("\u001b[31mGeneral Protection Fault Exception!\u001b[0m");
    println("        rip = {:#x}", frame->rip);
    println("         cs = {:#x}", frame->cs);
    println("     rflags = {:#x}", frame->rflags);
    println("        rsp = {:#x}", frame->rsp);
    println("         ss = {:#x}", frame->ss);
    println("      error = {:#x}", frame->error);
    println("  interrupt = {:#x}", frame->interrupt);
}

struct PACKED BacktraceFrame {
    BacktraceFrame *rbp;
    u64 rip;
};
static_assert(sizeof(BacktraceFrame) == 16);

EXCEPTION_HANDLER_WITH_CODE(14, page_fault_exception);
void page_fault_exception_handler(StackFrameErrorCode *frame) {
    u64 address = 0;
    asm volatile("movq %%cr2, %%rax"
                 : "=a"(address));
    println("\u001b[31mPage Fault Exception!\u001b[0m");
    println("    rip = {:#x}", frame->rip);
    println("     cs = {:#x}", frame->cs);
    println(" rflags = {:#x}", frame->rflags);
    println("    rbp = {:#x}", frame->rbp);
    println("    rsp = {:#x}", frame->rsp);
    println("     ss = {:#x}", frame->ss);
    println("  error = {:#x}", frame->error);
    println("address = {:#x}", address);

    println("\nBacktrace:");
    BacktraceFrame *stack_frame;
    asm("movq %%rbp, %0"
        : "=r"(stack_frame));

    for (int i = 0; i < 10; i++) {
        println("{}: rip = {:#x}", i, stack_frame->rip);

        if (stack_frame->rbp == nullptr || stack_frame->rbp == stack_frame)
            break;

        stack_frame = stack_frame->rbp;
    }
}

EXCEPTION_HANDLER(16, floating_point_error_exception);
void floating_point_error_exception_handler(StackFrame *frame) {
    println("Floating Point Exception!");
}

EXCEPTION_HANDLER_WITH_CODE(17, alignment_check_exception);
void alignment_check_exception_handler(StackFrameErrorCode *frame) {
    println("Alignment Check Exception!");
}

EXCEPTION_HANDLER(18, machine_check_exception);
void machine_check_exception_handler(StackFrame *frame) {
    println("Machine Check Exception!");
}

EXCEPTION_HANDLER(19, simd_floating_point_exception);
void simd_floating_point_exception_handler(StackFrame *frame) {
    println("SIMD Floating Point Exception!");
}

EXCEPTION_HANDLER(20, virtualisation_exception);
void virtualisation_exception_handler(StackFrame *frame) {
    println("Virtualisation Exception!");
}

void configure_exceptions(InterruptVectorTable &ivt) {
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
