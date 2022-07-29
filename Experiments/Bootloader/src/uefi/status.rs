/*
 * WorkbenchOS
 * Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

const ERROR_STATUS: u64 = 0x8000000000000000;

#[repr(u64)]
#[derive(Debug)]
#[allow(dead_code)]
pub enum Status {
    Success = 0,
    LoadError = ERROR_STATUS | 1,
    InvalidParameter = ERROR_STATUS | 2,
    Unsupported = ERROR_STATUS | 3,
    BadBufferSize = ERROR_STATUS | 4,
    BufferTooSmall = ERROR_STATUS | 5,
    NotReady = ERROR_STATUS | 6,
    DeviceError = ERROR_STATUS | 7,
    WriteProtected = ERROR_STATUS | 8,
    OutOfResources = ERROR_STATUS | 9,
    VolumeCorrupted = ERROR_STATUS | 10,
    VolumeFull = ERROR_STATUS | 11,
    NoMedia = ERROR_STATUS | 12,
    MediaChanged = ERROR_STATUS | 13,
    NotFound = ERROR_STATUS | 14,
    AccessDenied = ERROR_STATUS | 15,
    NoResponse = ERROR_STATUS | 16,
    NoMapping = ERROR_STATUS | 17,
    Timeout = ERROR_STATUS | 18,
    NotStarted = ERROR_STATUS | 19,
    AlreadyStarted = ERROR_STATUS | 20,
    Aborted = ERROR_STATUS | 21,
    IcmpError = ERROR_STATUS | 22,
    TftpError = ERROR_STATUS | 23,
    ProtocolError = ERROR_STATUS | 24,
    IncompatibleVersion = ERROR_STATUS | 25,
    SecurityViolation = ERROR_STATUS | 26,
    CrcError = ERROR_STATUS | 27,
    EndOfMedia = ERROR_STATUS | 28,
    EndOfFile = ERROR_STATUS | 31,
    InvalidLanguage = ERROR_STATUS | 32,
    CompromisedData = ERROR_STATUS | 33,
    IpAddressConflict = ERROR_STATUS | 34,
    HttpError = ERROR_STATUS | 35,
}
