/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;
use core::ptr;

use crate::{GuidParts};
use crate::uefi::protocol_handler::ProtocolHandler;
use crate::uefi::status::Status;
use crate::uefi::tables::{GraphicsOutputModeInformation, GraphicsOutputProtocolMode, GraphicsOutputProtocolTable, GUID};

pub struct GraphicsOutputProtocol {
    table: *const GraphicsOutputProtocolTable,
}

impl ProtocolHandler for GraphicsOutputProtocol {
    fn name() -> &'static str {
        "GraphicsOutputProtocol"
    }

    fn guid() -> GUID {
        GUID {
            parts: GuidParts {
                ms1: 0x9042a9de,
                ms2: 0x23dc,
                ms3: 0x4a38,
                ms4: [0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a],
            }
        }
    }

    fn create( table: *const c_void) -> Self {
        GraphicsOutputProtocol { table: table as *const GraphicsOutputProtocolTable }
    }
}

impl GraphicsOutputProtocol {
    pub fn mode(&self) -> Option<&GraphicsOutputProtocolMode> {
        unsafe {
            (*self.table).mode.as_ref()
        }
    }

    pub fn query_mode<'a>(&self, mode_number: u32) -> Option<&'a GraphicsOutputModeInformation> {
        let size_of_info: usize = 0;
        let mut ptr = ptr::null_mut();
        unsafe {
            ((*self.table).query_mode)(&*self.table, mode_number, &size_of_info, &mut ptr);
            ptr.as_ref()
        }
    }

    pub fn set_mode(&self, mode_number: u32) -> Result<(), Status> {
        unsafe {
            ((*self.table).set_mode)(&*self.table, mode_number);
        }
        Ok(())
    }
}
