/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#![macro_escape]

use core::arch::asm;
use core::fmt::Write;

fn outport_byte_ref(port: u16, value: &u8) {
    unsafe {
        asm!(
        "out dx, al",
        in("dx") port,
        in("al") *value,
        );
    }
}

pub fn outport_byte(port: u16, value: u8) {
    unsafe {
        asm!(
        "out dx, al",
        in("dx") port,
        in("al") value,
        );
    }
}

pub struct DebuggingConsoleWriter {}

impl Write for DebuggingConsoleWriter {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        for c in s.as_bytes() {
            outport_byte_ref(0xe9, c)
        }
        Ok(())
    }
}

#[macro_export]
macro_rules! debugln {
    ($($arg:tt)*) => {{
        use core::fmt;
        use crate::debug::DebuggingConsoleWriter;
        use crate::debug::outport_byte;

        let mut output = DebuggingConsoleWriter{};
        fmt::write(&mut output, format_args!($($arg)*)).unwrap();
        outport_byte(0xe9, b'\n');
    }};
}
