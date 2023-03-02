/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */


use crate::{debugln, ivt};

extern "x86-interrupt" fn divide_by_zero_exception(stack_frame: ivt::StackFrame) {
    debugln!("divide_by_zero_exception");
    loop {}
}

extern "x86-interrupt" fn debug_exception(stack_frame: ivt::StackFrame) {
    debugln!("debug_exception");
    loop {}
}

extern "x86-interrupt" fn nmi_interrupt(stack_frame: ivt::StackFrame) {
    debugln!("nmi_interrupt");
    loop {}
}

extern "x86-interrupt" fn breakpoint_exception(stack_frame: ivt::StackFrame) {
    debugln!("breakpoint_exception");
    loop {}
}

extern "x86-interrupt" fn overflow_exception(stack_frame: ivt::StackFrame) {
    debugln!("overflow_exception");
    loop {}
}

extern "x86-interrupt" fn bound_range_exceeded_exception(stack_frame: ivt::StackFrame) {
    debugln!("bound_range_exceeded_exception");
    loop {}
}

extern "x86-interrupt" fn invalid_opcode_exception(stack_frame: ivt::StackFrame) {
    debugln!("invalid_opcode_exception");
    loop {}
}

extern "x86-interrupt" fn device_not_available_exception(stack_frame: ivt::StackFrame) {
    debugln!("device_not_available_exception");
    loop {}
}

extern "x86-interrupt" fn double_fault_exception(stack_frame: ivt::StackFrame) {
    debugln!("double_fault_exception");
    loop {}
}

extern "x86-interrupt" fn coprocessor_segment_overrun_exception(stack_frame: ivt::StackFrame) {
    debugln!("coprocessor_segment_overrun_exception");
    loop {}
}

extern "x86-interrupt" fn invalid_tss_exception(stack_frame: ivt::StackFrame) {
    debugln!("invalid_tss_exception");
    loop {}
}

extern "x86-interrupt" fn segment_not_present_exception(stack_frame: ivt::StackFrame) {
    debugln!("segment_not_present_exception");
    loop {}
}

extern "x86-interrupt" fn stack_fault_exception(stack_frame: ivt::StackFrame) {
    debugln!("stack_fault_exception");
    loop {}
}

extern "x86-interrupt" fn general_protection_fault_exception(stack_frame: ivt::StackFrame) {
    debugln!("general_protection_fault_exception");
    loop {}
}

extern "x86-interrupt" fn page_fault_exception(stack_frame: ivt::StackFrameWithCode) {
    debugln!("page_fault_exception");
    debugln!("error_code: {:x}", stack_frame.error_code);
    debugln!("       rip: {:x}", stack_frame.instruction_pointer);
    debugln!("        cs: {:x}", stack_frame.code_segment);
    debugln!("     flags: {:x}", stack_frame.cpu_flags);
    debugln!("       rsp: {:x}", stack_frame.stack_pointer);
    debugln!("        ss: {:x}", stack_frame.stack_segment);
    loop {}
}

extern "x86-interrupt" fn floating_point_error_exception(stack_frame: ivt::StackFrame) {
    debugln!("floating_point_error_exception");
    loop {}
}

extern "x86-interrupt" fn alignment_check_exception(stack_frame: ivt::StackFrame) {
    debugln!("alignment_check_exception");
    loop {}
}

extern "x86-interrupt" fn machine_check_exception(stack_frame: ivt::StackFrame) {
    debugln!("machine_check_exception");
    loop {}
}

extern "x86-interrupt" fn simd_floating_point_exception(stack_frame: ivt::StackFrame) {
    debugln!("simd_floating_point_exception");
    loop {}
}

extern "x86-interrupt" fn virtualisation_exception(stack_frame: ivt::StackFrame) {
    debugln!("virtualisation_exception");
    loop {}
}

pub fn initialise() {
    ivt::set_interrupt_gate(0, ivt::PrivilegeLevel::Kernel, divide_by_zero_exception);
    ivt::set_interrupt_gate(1, ivt::PrivilegeLevel::Kernel, debug_exception);
    ivt::set_interrupt_gate(2, ivt::PrivilegeLevel::Kernel, nmi_interrupt);
    ivt::set_interrupt_gate(3, ivt::PrivilegeLevel::Kernel, breakpoint_exception);
    ivt::set_interrupt_gate(4, ivt::PrivilegeLevel::Kernel, overflow_exception);
    ivt::set_interrupt_gate(5, ivt::PrivilegeLevel::Kernel, bound_range_exceeded_exception);
    ivt::set_interrupt_gate(6, ivt::PrivilegeLevel::Kernel, invalid_opcode_exception);
    ivt::set_interrupt_gate(7, ivt::PrivilegeLevel::Kernel, device_not_available_exception);
    ivt::set_interrupt_gate(8, ivt::PrivilegeLevel::Kernel, double_fault_exception);
    ivt::set_interrupt_gate(9, ivt::PrivilegeLevel::Kernel, coprocessor_segment_overrun_exception);
    ivt::set_interrupt_gate(10, ivt::PrivilegeLevel::Kernel, invalid_tss_exception);
    ivt::set_interrupt_gate(11, ivt::PrivilegeLevel::Kernel, segment_not_present_exception);
    ivt::set_interrupt_gate(12, ivt::PrivilegeLevel::Kernel, stack_fault_exception);
    ivt::set_interrupt_gate(13, ivt::PrivilegeLevel::Kernel, general_protection_fault_exception);
    ivt::set_interrupt_gate_with_code(14, ivt::PrivilegeLevel::Kernel, page_fault_exception);
    ivt::set_interrupt_gate(16, ivt::PrivilegeLevel::Kernel, floating_point_error_exception);
    ivt::set_interrupt_gate(17, ivt::PrivilegeLevel::Kernel, alignment_check_exception);
    ivt::set_interrupt_gate(18, ivt::PrivilegeLevel::Kernel, machine_check_exception);
    ivt::set_interrupt_gate(19, ivt::PrivilegeLevel::Kernel, simd_floating_point_exception);
    ivt::set_interrupt_gate(20, ivt::PrivilegeLevel::Kernel, virtualisation_exception);
}
