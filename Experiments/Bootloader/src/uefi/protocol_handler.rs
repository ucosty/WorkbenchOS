/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

use core::ffi::c_void;

use crate::uefi::tables::GUID;

pub trait ProtocolHandler {
    fn name() -> &'static str;
    fn guid() -> GUID;
    fn create(table: *const c_void) -> Self;
}
