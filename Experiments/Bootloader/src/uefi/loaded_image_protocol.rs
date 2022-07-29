/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;
use crate::{GuidParts, Handle};
use crate::uefi::protocol_handler::ProtocolHandler;
use crate::uefi::tables::{GUID, LoadedImageProtocolTable};

pub struct LoadedImageProtocol {
    table: *const LoadedImageProtocolTable
}

impl ProtocolHandler for LoadedImageProtocol {
    fn name() -> &'static str {
        "LoadedImageProtocol"
    }

    fn guid() -> GUID {
        GUID {
            parts: GuidParts {
                ms1: 0x5b1b31a1,
                ms2: 0x9562,
                ms3: 0x11d2,
                ms4: [0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b],
            }
        }
    }

    fn create(table: *const c_void) -> Self {
        LoadedImageProtocol { table: table as *const LoadedImageProtocolTable }
    }
}

impl LoadedImageProtocol {
    pub fn device_handle(&self) -> Handle {
        unsafe { (*self.table).device_handle }
    }
}
