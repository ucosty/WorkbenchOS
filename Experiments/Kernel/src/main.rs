#![feature(abi_x86_interrupt)]
/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#![no_std]
#![no_main]

mod debug;
mod boot;
mod platform;
mod ivt;
mod exceptions;
mod apic;
mod memory_types;
mod gdt;

use core::panic::PanicInfo;
use boot::BootState;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    debugln!("{}", _info);
    loop {}
}

fn init(boot_state: &BootState) {
    debugln!("This is a test: {} MB RAM", boot_state.physical_memory_size / 1048576);
    debugln!("Screen resolution: {}x{}", boot_state.framebuffer.width, boot_state.framebuffer.height);

    platform::disable_pic();
    ivt::initialise();
    exceptions::initialise();
    apic::initialise();
}


/*
extern "C" [[noreturn]] EFICALL void kernel_main(u64 boot_state_address) {
    gdt_pointer.address = reinterpret_cast<u64>(&segments);
    gdt_pointer.limit = sizeof(SegmentDescriptor) * gdt_descriptors;

    auto tss0_address = reinterpret_cast<u64>(&tss0);
    TSSDescriptor tss_descriptor_0{
        .segment_limit = sizeof(TSS) - 1,
        .base_low = tss0_address & 0xffff,
        .base_low_middle = (tss0_address >> 16) & 0xff,
        .segment_type = 9,
        .privilege_level = 0,
        .present = 1,
        .granularity = 0,
        .base_high_middle = (tss0_address >> 24) & 0xff,
        .base_high = (u32) (tss0_address >> 32),
    };

    segments[5] = tss_descriptor_0.low;
    segments[6] = tss_descriptor_0.high;
    asm volatile("lgdt gdt_pointer\n"
                 "mov %%rax, %%rdi\n"// Boot state block
                 "mov $0x10, %%rbx\n"
                 "mov %%rbx, %%ss\n"
                 "mov %%rbx, %%ds\n"
                 "mov %%rbx, %%es\n"
                 "mov %%rbx, %%fs\n"
                 "mov %%rbx, %%gs\n"
                 "movq $0x0, %%rbp\n"
                 "pushq $0x08\n"         // Push CS
                 "pushq $kernel_stage2\n"// Push RIP
                 "lretq\n"
                 : /* no output */
                 : "a"(boot_state_address));
    Processor::halt();
}
 */

#[no_mangle]
pub extern "efiapi" fn _start(boot_state: &BootState) -> ! {
    init(boot_state);
    panic!("You should not be here");
}
