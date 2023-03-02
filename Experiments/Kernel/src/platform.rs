/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::arch::asm;
use core::arch::x86_64::{__cpuid_count, CpuidResult};
use core::slice;
use crate::debugln;

pub fn disable_pic() {
    unsafe {
        asm!(
        "mov $0xff, %al\n",
        "outb %al, $0xa1\n",
        "outb %al, $0x21\n",
        options(att_syntax)
        );
    }
}

pub fn enable_interrupts() {
    unsafe {
        asm!("sti");
    }
}

pub fn disable_interrupts() {
    unsafe {
        asm!("cli");
    }
}

pub fn lidt(idt: usize) {
    unsafe {
        asm!("lidt ({0})", in(reg) idt, options(att_syntax));
    }
}

pub enum CpuidFunction {
    HighestFunctionAndManufacturerId = 0,
    ProcessorInfo = 1,
    CacheAndTLBDescriptor = 2,
    ProcessorSerialNumber = 3,
    IntelCoreAndCacheTopology = 4,
    ThermalAndPowerManagement = 6,
}

pub struct VendorString {
    ebx: u32,
    edx: u32,
    ecx: u32,
}

pub struct ProcessorString {
    parts: [u32; 12],
}

pub fn get_vendor_string() {
    // let registers = cpuid(0x80000000, 0);
    let registers = cpuid(0, 0);
    let vendor = VendorString {
        ebx: registers.ebx,
        edx: registers.edx,
        ecx: registers.ecx,
    };

    debugln!("Highest function {:x}", registers.eax);
    debugln!("Got vendor string: {:x} {:x} {:x}", vendor.ebx, vendor.edx, vendor.ecx);

    let vendor_string = unsafe {
        slice::from_raw_parts((&vendor as *const _) as *const u8, 12)
    };
}


pub fn get_processor_name_string() {
    // 8000_0002h, 8000_0003h, and 8000_0004h
    let processor_string = {
        let part_1 = cpuid(0x80000002, 0);
        let part_2 = cpuid(0x80000003, 0);
        let part_3 = cpuid(0x80000004, 0);
        ProcessorString {
            parts: [
                part_1.eax,
                part_1.ebx,
                part_1.ecx,
                part_1.edx,
                part_2.eax,
                part_2.ebx,
                part_2.ecx,
                part_2.edx,
                part_3.eax,
                part_3.ebx,
                part_3.ecx,
                part_3.edx,
            ]
        }
    };
}

pub fn cpuid(leaf: u32, sub_leaf: u32) -> CpuidResult {
    unsafe {
        __cpuid_count(leaf, sub_leaf)
    }
}

pub fn has_apic() -> bool {
    let feature_bits = cpuid(1, 0);
    (feature_bits.edx >> 9) & 1 == 1
}

pub fn read_msr(msr: u32) -> u64 {
    let (high, low): (u32, u32);
    unsafe {
        asm!("rdmsr",
        out("eax") low,
        out("edx") high,
        in("ecx") msr);
    }
    ((high as u64) << 32) | (low as u64)
}