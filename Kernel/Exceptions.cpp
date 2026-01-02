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

struct FramePointer {
    FramePointer* next;
    u64           ret;
};

static bool is_canonical(const u64 address) {
    const u64 sign = (address >> 47) & 1;
    const u64 top  = address >> 48;
    return sign ? (top == 0xFFFF) : (top == 0x0000);
}

EXCEPTION_HANDLER_WITH_CODE(14, page_fault_exception);
void page_fault_exception_handler(StackFrameErrorCode *frame) {
    println("\u001b[31mPage Fault Exception!\u001b[0m");
    uint64_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2) :: "memory");

    println("    rip = {:#x}", frame->rip);
    println("     cs = {:#x}", frame->cs);
    println(" rflags = {:#x}", frame->rflags);
    println("    rbp = {:#x}", frame->rbp);
    println("    rsp = {:#x}", frame->rsp);
    println("     ss = {:#x}", frame->ss);
    println("  error = {:#x}", frame->error);
    println("address = {:#x}", cr2);

    println("\nBacktrace:");

    // First entry: the faulting RIP itself (this is *not* a return address).
    println("fault: rip = {:#x}", frame->rip);

    // Start from the *faulting context* RBP, not the handler's RBP.
    auto* fp = reinterpret_cast<FramePointer*>(frame->rbp);

    for (int i = 0; i < 15; i++) {
        if (!fp) break;

        uint64_t fp_addr = reinterpret_cast<uint64_t>(fp);
        if (!is_canonical(fp_addr) || (fp_addr & 0x7)) break; // basic sanity

        // Optional but strongly recommended:
        // if (!in_stack_range(fp_addr)) break;

        // Read next + return address.
        // NOTE: This assumes the memory at fp is mapped; if you still sometimes
        // fault here, you need either a "safe read" or ensure stacks are mapped.
        FramePointer* next = fp->next;
        uint64_t ret = fp->ret;

        if (!is_canonical(ret) || ret == 0) break;

        // ret points *after* the call. Subtract 1 to land within the call site.
        uint64_t ip = ret - 1;

        println("{}: rip = {:#x} (ret={:#x}, rbp={:#x})",
                i, ip, ret, fp_addr);

        // Stop on obvious loops / non-progress.
        if (next == fp) break;
        if (reinterpret_cast<uint64_t>(next) <= fp_addr) break; // stack should grow down, rbp should increase as you unwind

        fp = next;
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
    InterruptVectorTable::set_interrupt_gate(0, PrivilegeLevel::Kernel, 0, &divide_by_zero_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(1, PrivilegeLevel::Kernel, 0, &debug_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(2, PrivilegeLevel::Kernel, 0, &nmi_interrupt_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(3, PrivilegeLevel::Kernel, 0, &breakpoint_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(4, PrivilegeLevel::Kernel, 0, &overflow_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(5, PrivilegeLevel::Kernel, 0, &bound_range_exceeded_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(6, PrivilegeLevel::Kernel, 0, &invalid_opcode_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(7, PrivilegeLevel::Kernel, 0, &device_not_available_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(8, PrivilegeLevel::Kernel, 2, &double_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(9, PrivilegeLevel::Kernel, 0, &coprocessor_segment_overrun_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(10, PrivilegeLevel::Kernel, 0, &invalid_tss_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(11, PrivilegeLevel::Kernel, 0, &segment_not_present_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(12, PrivilegeLevel::Kernel, 0, &stack_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(13, PrivilegeLevel::Kernel, 0, &general_protection_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(14, PrivilegeLevel::Kernel, 1, &page_fault_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(16, PrivilegeLevel::Kernel, 0, &floating_point_error_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(17, PrivilegeLevel::Kernel, 0, &alignment_check_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(18, PrivilegeLevel::Kernel, 0, &machine_check_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(19, PrivilegeLevel::Kernel, 0, &simd_floating_point_exception_asm_wrapper);
    InterruptVectorTable::set_interrupt_gate(20, PrivilegeLevel::Kernel, 0, &virtualisation_exception_asm_wrapper);
}
