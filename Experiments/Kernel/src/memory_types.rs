/*
 * WorkbenchOS
 * Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

const PHYSICAL_MAPPING_BASE: u64 = 0xffffff0000000000;

pub struct PhysicalAddress {
    address: u64,
}

impl PhysicalAddress {
    pub fn new(address: u64) -> PhysicalAddress {
        PhysicalAddress{address}
    }

    pub fn as_mapped_address(&self) -> u64 {
        return self.address + PHYSICAL_MAPPING_BASE;
    }

    pub fn as_type<T>(&self) -> *const T {
        let mapped_address = self.as_mapped_address();
        unsafe { mapped_address as *const T }
    }

    pub fn as_mut_type<T>(&self) -> *mut T {
        let mapped_address = self.as_mapped_address();
        unsafe { mapped_address as *mut T }
    }
}
