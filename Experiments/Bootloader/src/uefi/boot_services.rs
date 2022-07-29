/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;
use core::ptr;
use core::ptr::null;

use crate::{BootServicesTable, Handle, SystemTable};
use crate::uefi::protocol_handler::ProtocolHandler;
use crate::uefi::status::Status;
use crate::uefi::tables::{GUID, MemoryType};

const SYSTEM_TABLE_SIGNATURE: u64 = 0x5453595320494249;

pub struct BootServices {
    boot_services: *const BootServicesTable,
}

#[derive(Debug)]
pub enum BootServicesError {
    InvalidSystemHeaderSignature,
    HandleProtocolError(Status, &'static str, GUID),
    LocateProtocolError(Status, &'static str, GUID),
}

impl BootServices {
    pub fn from_system_table(system_table: &SystemTable) -> Result<BootServices, BootServicesError> {
        if system_table.header.signature != SYSTEM_TABLE_SIGNATURE {
            return Err(BootServicesError::InvalidSystemHeaderSignature);
        }
        Ok(BootServices { boot_services: system_table.boot_services })
    }

    pub fn disable_watchdog_timer(&self) -> Result<(), Status> {
        let status = unsafe {
            ((*self.boot_services).set_watchdog_timer)(0, 0, 0, null())
        };
        match status {
            Status::Success => Ok(()),
            _ => Err(status)
        }
    }

    pub fn handle_protocol<Handler: ProtocolHandler>(&self, handle: Handle) -> Result<Handler, BootServicesError> {
        let mut ptr = ptr::null_mut();
        let guid = Handler::guid();
        let status = unsafe {
            ((*self.boot_services).handle_protocol)(handle, &guid, &mut ptr)
        };
        match status {
            Status::Success => Ok(Handler::create(ptr)),
            _ => Err(BootServicesError::HandleProtocolError(status, Handler::name(), Handler::guid()))
        }
    }

    pub fn locate_protocol<Handler: ProtocolHandler>(&self) -> Result<Handler, BootServicesError> {
        let mut ptr = ptr::null_mut();
        let guid = Handler::guid();
        let status = unsafe {
            ((*self.boot_services).locate_protocol)(&guid, null(), &mut ptr)
        };
        match status {
            Status::Success => Ok(Handler::create(ptr)),
            _ => Err(BootServicesError::LocateProtocolError(status, Handler::name(), Handler::guid()))
        }
    }

    pub fn allocate_pool(&self, pool_type: MemoryType, size: u64) -> Result<*mut c_void, Status> {
        let mut ptr = ptr::null_mut();
        let status = unsafe { ((*self.boot_services).allocate_pool)(pool_type, size, &mut ptr) };
        match status {
            Status::Success => Ok(ptr),
            _ => Err(status)
        }
    }
}
