/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::arch::asm;
use core::ffi::c_void;
use core::mem::size_of;
use core::ptr;
use crate::debugln;

#[repr(C, align(1))]
pub struct MemoryDescriptor {
    pub limit: u16,
    pub address: *mut c_void,
}

#[repr(C, align(1))]
#[derive(Copy, Clone)]
pub struct InterruptDescriptor {
    pub low: u64,
    pub high: u64,
}

static mut g_idt_pointer: MemoryDescriptor = MemoryDescriptor { limit: (size_of::<InterruptDescriptor>() * 256) as u16, address: ptr::null_mut() };
static mut g_interrupts: [InterruptDescriptor; 256] = [InterruptDescriptor { low: 0, high: 0 }; 256];

enum PrivilegeLevel {
    Kernel,
    User,
}

#[repr(C)]
pub struct StackFrame {
    pub instruction_pointer: u64,
    pub code_segment: u64,
    pub cpu_flags: u64,
    pub stack_pointer: u64,
    pub stack_segment: u64,
}

extern "x86-interrupt" fn handler(stack_frame: StackFrame) {
    debugln!("Got interrupt");
    debugln!("RIP = {}", stack_frame.instruction_pointer);
    debugln!(" CS = {}", stack_frame.code_segment);
    debugln!(" SS = {}", stack_frame.stack_segment);
}

pub fn initialise() {
    for i in 0..255 {
        set_interrupt_gate(i, PrivilegeLevel::Kernel, &handler as *mut c_void);
    }

    // set_interrupt_gate(32, PrivilegeLevel::User, &handler as *const c_void);
    // set_interrupt_gate(33, PrivilegeLevel::Kernel, &handler as *const c_void);
    // set_interrupt_gate(34, PrivilegeLevel::Kernel, &handler as *const c_void);

    unsafe {
        g_idt_pointer.address = &g_interrupts as *mut c_void;
        asm!("lidt g_idt_pointer");
    }
}

fn set_interrupt_gate(id: u8, privilege_level: PrivilegeLevel, handler: *mut c_void) {
    let address = handler as usize;
    let offset_1: u16 = (address & 0xffff) as u16;
    let offset_2: u16 = ((address >> 16) & 0xffff) as u16;
    let offset_3: u32 = (address >> 32) as u32;
    let segment_selector: u16 = 0x08;
    let kind: u8 = 0x0f;
    let privilege_level: u8 = match privilege_level {
        PrivilegeLevel::Kernel => 0x00,
        PrivilegeLevel::User => 0x03,
    };

    unsafe {
        g_interrupts[id as usize].low = (offset_1 as u64)
            | (segment_selector as u64) << 16
            | (kind as u64) << 40
            | (privilege_level as u64) << 45
            | 1 << 47
            | (offset_2 as u64) << 48;
        g_interrupts[id as usize].high = offset_3 as u64;
    }
}
