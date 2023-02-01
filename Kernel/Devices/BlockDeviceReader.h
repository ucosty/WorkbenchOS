// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Devices/BlockDevice.h>
#include <LibStd/String.h>


enum class BlockDeviceErrorCode {
    ReadError
};

class BlockDeviceError {
public:
    BlockDeviceError(BlockDeviceErrorCode code) : m_code(code) {}
private:
    BlockDeviceErrorCode m_code;
};

class BlockDeviceReader {
public:
    BlockDeviceReader(BlockDevice *block_device) : m_block_device(block_device) {}

    Std::Result<u16> read_uint16();

    Std::Result<void> read_buffer(u8 *destination, size_t size);

    Std::Result<Std::String> read_string(size_t size);

    Std::Result<void> seek(size_t offset);

    template<typename T>
    Std::Result<T> read() {
        T buffer{};
        m_block_device->read(m_offset, sizeof(T), reinterpret_cast<u8 *>(&buffer));
        m_offset += sizeof(T);
        return buffer;
    }

private:
    BlockDevice *m_block_device;
    size_t m_offset{0};
};
