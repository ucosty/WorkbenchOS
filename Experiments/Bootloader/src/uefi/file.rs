/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;
use core::ptr;
use core::ptr::null_mut;

use crate::{BootServices, GuidParts, Status};
use crate::uefi::protocol_handler::ProtocolHandler;
use crate::uefi::tables::{FileInfoTable, FileTable, GUID, MemoryType};

pub enum Mode {
    Read = 0x01
}

pub struct File {
    pub(crate) table: *const FileTable,
}

pub struct FileInfo {
    pub table: *const FileInfoTable,
}

impl ProtocolHandler for FileInfo {
    fn name() -> &'static str {
        "FileInfo"
    }

    fn guid() -> GUID {
        GUID {
            parts: GuidParts {
                ms1: 0x09576e92,
                ms2: 0x6d3f,
                ms3: 0x11d2,
                ms4: [0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b],
            }
        }
    }

    fn create( table: *const c_void) -> Self {
        FileInfo { table: table as *const FileInfoTable }
    }
}

impl FileInfo {
    pub fn size(&self) -> u64 {
        unsafe { (*self.table).size }
    }
}

impl File {
    pub fn create(table: *const c_void) -> File {
        File { table: table as *const FileTable }
    }

    pub fn open(&self, filename: *const u16, mode: Mode, attributes: Mode) -> Result<File, Status> {
        let mut ptr = ptr::null_mut();
        let status = unsafe {
            ((*self.table).open)(&*self.table, &mut ptr, filename, mode as u64, attributes as u64)
        };
        match status {
            Status::Success => return Ok(File::create(ptr)),
            _ => Err(status)
        }
    }

    pub fn get_info<Type: ProtocolHandler>(&self, boot_services: &BootServices) -> Result<Type, Status> {
        let buffer_size = 0u64;
        unsafe {
            ((*self.table).get_info)(&*self.table, &Type::guid(), &buffer_size, null_mut());
        }
        let ptr = boot_services.allocate_pool(MemoryType::LoaderData, buffer_size)?;
        let status = unsafe { ((*self.table).get_info)(&*self.table, &Type::guid(), &buffer_size, ptr) };
        match status {
            Status::Success => return Ok(Type::create(ptr)),
            _ => Err(status)
        }
    }

    pub fn read_bytes(&self, boot_services: &BootServices, size: u64) -> Result<*mut c_void, Status> {
        let ptr = boot_services.allocate_pool(MemoryType::LoaderData, size)?;
        let status = unsafe { ((*self.table).read)(&*self.table, &size, ptr) };
        match status {
            Status::Success => Ok(ptr),
            _ => Err(status)
        }
    }
}
