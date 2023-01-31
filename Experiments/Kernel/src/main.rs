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

use core::panic::PanicInfo;
use core::slice;
use boot::BootState;
use crate::boot::MemoryDescriptor;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    debugln!("{}", _info);
    loop {}
}


#[no_mangle]
pub extern "efiapi" fn _start(boot_state: &BootState) -> ! {
    debugln!("This is a test: {} MB RAM", boot_state.physical_memory_size / 1048576);
    debugln!("Screen resolution: {}x{}", boot_state.framebuffer.width, boot_state.framebuffer.height);

    platform::disable_pic();

    unsafe {
        let ptr: *const MemoryDescriptor = boot_state.memory_map.descriptors as *const MemoryDescriptor;
        let descriptors = unsafe { slice::from_raw_parts(ptr, boot_state.memory_map.descriptor_count as usize) };
        // for descriptor in descriptors.iter() {
        // debugln!("{}: start = {:#x}, size = {:#x}", descriptor.kind, descriptor.physical_start, descriptor.number_of_pages * 0x1000);
        // }
        debugln!("{}: start = {:#x}, size = {:#x}", descriptors[0].kind, descriptors[0].physical_start, descriptors[0].number_of_pages * 0x1000);
    }
    debugln!("Test test test");
    panic!("Nobody lives here");
}
