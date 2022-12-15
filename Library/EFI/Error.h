// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Error.h>
#include <LibStd/Result.h>

namespace EFI {
enum class ErrorType {
    LoadError = 1,
    InvalidParameter = 2,
    Unsupported = 3,
    BadBufferSize = 4,
    BufferTooSmall = 5,
    NotReady = 6,
    DeviceError = 7,
    WriteProtected = 8,
    OutOfResources = 9,
    VolumeCorrupted = 10,
    VolumeFull = 11,
    NoMedia = 12,
    MediaChanged = 13,
    NotFound = 14,
    AccessDenied = 15,
    NoResponse = 16,
    NoMapping = 17,
    Timeout = 18,
    NotStarted = 19,
    AlreadyStarted = 20,
    Aborted = 21,
    IcmpError = 22,
    TftpError = 23,
    ProtocolError = 24,
    IncompatibleVersion = 25,
    SecurityViolation = 26,
    CrcError = 27,
    EndOfMedia = 28,
    EndOfFile = 31,
    InvalidLanguage = 32,
    CompromisedData = 33,
    IpAddressConflict = 34,
    HttpError = 35,
    InvalidMemoryMap = 1000
};

Std::StringView error_to_string(ErrorType type);

class Error {
public:
    explicit Error(ErrorType type) : m_type(type) {}
    explicit Error(uint64_t code) : m_type(static_cast<ErrorType>(code)) {}

    template<typename T>
    operator Std::Result<T, Std::Error>() {
        return Std::Error::with_message(error_to_string(m_type));
    }

    [[nodiscard]] ErrorType get_type() const { return m_type; }
private:
    ErrorType m_type;
};
}
