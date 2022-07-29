/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;
use core::ptr;

use crate::{GuidParts, Status};
use crate::uefi::file::File;
use crate::uefi::protocol_handler::ProtocolHandler;
use crate::uefi::tables::{GUID, SimpleFileSystemProtocolTable};

pub struct SimpleFileSystemProtocol {
    table: *const SimpleFileSystemProtocolTable,
}

impl ProtocolHandler for SimpleFileSystemProtocol {
    fn name() -> &'static str {
        "SimpleFileSystemProtocol"
    }

    fn guid() -> GUID {
        GUID {
            parts: GuidParts {
                ms1: 0x964e5b22,
                ms2: 0x6459,
                ms3: 0x11d2,
                ms4: [0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b],
            }
        }
    }

    fn create(table: *const c_void) -> Self {
        SimpleFileSystemProtocol { table: table as *const SimpleFileSystemProtocolTable }
    }
}

impl SimpleFileSystemProtocol {
    pub fn open_volume(&self) -> Result<File, Status> {
        let mut ptr = ptr::null_mut();
        let status = unsafe {
            ((*self.table).open_volume)(&*self.table, &mut ptr)
        };
        match status {
            Status::Success => Ok(File::create(ptr)),
            _ => Err(status)
        }
    }
}
