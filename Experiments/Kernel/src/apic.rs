/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::arch::asm;
use core::ptr;
use crate::memory_types::PhysicalAddress;
use crate::{debugln, platform};
use crate::platform::enable_interrupts;

/*
struct PACKED SpuriousVector {
    u32 vector : 8;
    u32 apic_software_enabled : 1;
    u32 focus_processor_checking : 1;
    u32 ignored : 2;
    u32 eoi_broadcast_suppression : 1;
    u32 ignored_2 : 19;
};
 */

#[repr(C, packed)]
struct SpuriousVector {
    vector: u8,
}

enum Polarity {
    ActiveHigh = 0,
    ActiveLow = 1,
}

enum DeliveryMode {
    Fixed = 0,
    SMI = 2,
    NMI = 4,
    INIT = 5,
    ExtINT = 7,
}

enum TriggerMode {
    EdgeSensitive = 0,
    LevelSensitive = 1,
}

enum Mask {
    Enabled = 0,
    Inhibited = 1,
}

struct LintVector {
    address: PhysicalAddress,
}

impl LintVector {
    pub fn new(address: u64) -> LintVector {
        LintVector { address: PhysicalAddress::new(address) }
    }

    pub fn set(&self, vector: u8, delivery_mode: DeliveryMode, interrupt_input_pin_polarity: Polarity, trigger_mode: TriggerMode, mask: Mask) {
        let data = self.address.as_mut_type::<u32>();

        let value = 0
            | vector as u32
            | (delivery_mode as u32) << 7
            | (interrupt_input_pin_polarity as u32) << 12
            | (trigger_mode as u32) << 14
            | (mask as u32) << 15;

        unsafe {
            ptr::write_volatile(data, value);
        }
    }
}

enum ApicSoftwareEnable {
    Disabled = 0,
    Enabled = 1,
}

enum EOIBroadcastSuppression {
    Disabled = 0,
    Enabled = 1,
}

enum FocusProcessorChecking {
    Enabled = 0,
    Disabled = 1,
}

struct SpuriousVectorRegister {
    address: PhysicalAddress,
}

impl SpuriousVectorRegister {
    pub fn new(address: u64) -> SpuriousVectorRegister {
        SpuriousVectorRegister { address: PhysicalAddress::new(address) }
    }

    pub fn set(&self, vector: u8, apic_enabled: ApicSoftwareEnable, focus_processor_checking: FocusProcessorChecking, suppress_eoi_broadcasts: EOIBroadcastSuppression) {
        let data = self.address.as_mut_type::<u32>();

        let value = 0
            | vector as u32
            | (apic_enabled as u32) << 7
            | (focus_processor_checking as u32) << 8
            | (suppress_eoi_broadcasts as u32) << 11;

        unsafe {
            ptr::write_volatile(data, value);
        }
    }
}

struct IoApic {
    base_ptr: *mut u32,
}

impl IoApic {
    pub fn new(register_base: usize) -> IoApic {
        let base_ptr = PhysicalAddress::new(register_base as u64).as_mut_type::<u32>();
        IoApic { base_ptr }
    }

    pub fn route_irq(&self, irq: u8, vector: u8) {
        let value = self.read(irq as u32) & 0xffffff00 | irq as u32;
        self.write(irq as u32, value);
        self.write((irq as u32) + 1, 0);
    }

    fn read(&self, register: u32) -> u32 {
        unsafe {
            ptr::write_volatile(self.base_ptr, register & 0xff);
            ptr::read_volatile(self.base_ptr.offset(4))
        }
    }

    fn write(&self, register: u32, value: u32) {
        unsafe {
            ptr::write_volatile(self.base_ptr, register & 0xff);
            ptr::write_volatile(self.base_ptr.offset(4), value);
        }
    }
}

pub fn initialise() {
    if !platform::has_apic() {
        return;
    }

    let register_base = {
        let apic_base_msr = platform::read_msr(0x1B);
        apic_base_msr & 0xffffffffff000
    };

    let lint0 = LintVector::new(register_base + 0x350);
    let lint1 = LintVector::new(register_base + 0x360);

    lint0.set(0x30, DeliveryMode::Fixed, Polarity::ActiveHigh, TriggerMode::EdgeSensitive, Mask::Enabled);
    lint1.set(0x30, DeliveryMode::Fixed, Polarity::ActiveHigh, TriggerMode::EdgeSensitive, Mask::Enabled);

    let spurious_vector_register = SpuriousVectorRegister::new(register_base + 0xf0);
    spurious_vector_register.set(0xff, ApicSoftwareEnable::Enabled, FocusProcessorChecking::Enabled, EOIBroadcastSuppression::Disabled);

    // FIXME: look up the IOAPIC address in the Multiple APIC Description Table
    let io_apic = IoApic::new(0xfec00000);

    io_apic.route_irq(0x12, 0x21);
    io_apic.route_irq(0x28, 0x22);
    enable_interrupts();

    debugln!("Interrupts enabled!");
    loop {}
}
