/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#![no_std]
#![no_main]

mod debug;

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub extern "C" fn _start() -> ! {
    debugln!("This is a test");
    loop {}
}
