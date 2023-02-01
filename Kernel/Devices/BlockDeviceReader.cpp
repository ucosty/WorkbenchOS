// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "BlockDeviceReader.h"
#include "UnbufferedConsole.h"
#include <LibStd/Try.h>

Std::Result<u16> BlockDeviceReader::read_uint16() {
    u16 result;
    TRY(m_block_device->read(m_offset, sizeof(u16), reinterpret_cast<u8 *>(&result)));
    m_offset += sizeof(u16);
    return result;
}

Std::Result<void> BlockDeviceReader::read_buffer(u8 *destination, size_t size) {
    TRY(m_block_device->read(m_offset, size, destination));
    m_offset += size;
    return {};
}

Std::Result<Std::String> BlockDeviceReader::read_string(size_t size) {
    char *destination = new char[size];
    TRY(m_block_device->read(m_offset, size, reinterpret_cast<u8 *>(destination)));
    m_offset += size;
    return Std::String(destination, size);
}

Std::Result<void> BlockDeviceReader::seek(size_t offset) {
    m_offset = offset;
    return {};
}
