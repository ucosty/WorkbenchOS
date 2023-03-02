/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::mem::size_of;
use crate::{debugln, platform};
use bitflags::bitflags;

bitflags! {
    pub struct Flags: u8 {
        const PRESENT = 0x80;
        const INTERRUPT = 0x0e;
        const EXCEPTION = 0x0f;
    }
}

#[repr(C, packed)]
pub struct DescriptorTablePointer {
    pub limit: u16,
    pub address: usize,
}

#[derive(Copy, Clone, Debug)]
#[repr(C, packed)]
pub struct InterruptDescriptor {
    pub offset_1: u16,
    pub segment_selector: u16,
    pub interrupt_stack_table: u8,
    pub attributes: Flags,
    pub offset_2: u16,
    pub offset_3: u32,
    pub _zero: u32,
}

#[no_mangle]
static mut G_IDT_POINTER: DescriptorTablePointer = DescriptorTablePointer { limit: (size_of::<InterruptDescriptor>() * 256) as u16, address: 0 };

static mut G_INTERRUPTS: [InterruptDescriptor; 256] = [InterruptDescriptor {
    offset_1: 0,
    segment_selector: 0,
    interrupt_stack_table: 0,
    attributes: Flags { bits: 0 },
    offset_2: 0,
    offset_3: 0,
    _zero: 0,
}; 256];

pub enum PrivilegeLevel {
    Kernel,
    User,
}

#[repr(C)]
#[derive(Debug)]
pub struct StackFrame {
    pub instruction_pointer: u64,
    pub code_segment: u64,
    pub cpu_flags: u64,
    pub stack_pointer: u64,
    pub stack_segment: u64,
}

#[repr(C)]
#[derive(Debug)]
pub struct StackFrameWithCode {
    pub error_code: u64,
    pub instruction_pointer: u64,
    pub code_segment: u64,
    pub cpu_flags: u64,
    pub stack_pointer: u64,
    pub stack_segment: u64,
}

extern "x86-interrupt" fn handler(stack_frame: StackFrame) {
    debugln!("Got interrupt!");
    debugln!("RIP = {:x}", stack_frame.instruction_pointer);
    debugln!(" CS = {:x}", stack_frame.code_segment);
    debugln!(" SS = {:x}", stack_frame.stack_segment);
}

pub fn initialise() {
    // let handler_address = (handler as *mut c_void) as usize;
    for i in 0..255 {
        set_interrupt_gate(i, PrivilegeLevel::Kernel, handler);
    }

    unsafe {
        G_IDT_POINTER.address = (&G_INTERRUPTS as *const _) as usize;
        platform::lidt((&G_IDT_POINTER as *const _) as usize);
    }
}

pub fn set_interrupt_gate(id: usize, privilege_level: PrivilegeLevel, handler: extern "x86-interrupt" fn(StackFrame)) {
    let address = handler as usize;
    unsafe {
        G_INTERRUPTS[id].offset_1 = (address & 0xffff) as u16;
        G_INTERRUPTS[id].offset_2 = ((address >> 16) & 0xffff) as u16;
        G_INTERRUPTS[id].offset_3 = (address >> 32) as u32;
        G_INTERRUPTS[id].segment_selector = 0x28;
        G_INTERRUPTS[id].attributes = Flags::PRESENT | Flags::INTERRUPT;
    }
}

pub fn set_interrupt_gate_with_code(id: usize, privilege_level: PrivilegeLevel, handler: extern "x86-interrupt" fn(StackFrameWithCode)) {
    let address = handler as usize;
    unsafe {
        G_INTERRUPTS[id].offset_1 = (address & 0xffff) as u16;
        G_INTERRUPTS[id].offset_2 = ((address >> 16) & 0xffff) as u16;
        G_INTERRUPTS[id].offset_3 = (address >> 32) as u32;
        G_INTERRUPTS[id].segment_selector = 0x28;
        G_INTERRUPTS[id].attributes = Flags::PRESENT | Flags::INTERRUPT;
    }
}
