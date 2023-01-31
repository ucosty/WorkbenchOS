// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "BlockDeviceReader.h"
#include "UnbufferedConsole.h"
#include <LibStd/Try.h>

Std::Result<uint16_t> BlockDeviceReader::read_uint16() {
    uint16_t result;
    TRY(m_block_device->read(m_offset, sizeof(uint16_t), reinterpret_cast<uint8_t *>(&result)));
    m_offset += sizeof(uint16_t);
    return result;
}

Std::Result<void> BlockDeviceReader::read_buffer(uint8_t *destination, size_t size) {
    TRY(m_block_device->read(m_offset, size, destination));
    m_offset += size;
    return {};
}

Std::Result<Std::String> BlockDeviceReader::read_string(size_t size) {
    char *destination = new char[size];
    TRY(m_block_device->read(m_offset, size, reinterpret_cast<uint8_t *>(destination)));
    m_offset += size;
    return Std::String(destination, size);
}

Std::Result<void> BlockDeviceReader::seek(size_t offset) {
    m_offset = offset;
    return {};
}
