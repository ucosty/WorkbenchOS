// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

void divide_by_zero_exception();
void debug_exception();
void nmi_interrupt();
void breakpoint_exception();
void overflow_exception();
void bound_range_exceeded_exception();
void invalid_opcode_exception();
void device_not_available_exception();
void double_fault_exception();
void coprocessor_segment_overrun_exception();
void invalid_tss_exception();
void segment_not_present_exception();
void stack_fault_exception();
void general_protection_fault_exception();
void page_fault_exception();
void floating_point_error_exception();
void alignment_check_exception();
void machine_check_exception();
void simd_floating_point_exception();
void virtualisation_exception();
