// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <Devices/MemoryBlockDevice.h>
#include <LibStd/CString.h>

using namespace Std;

MemoryBlockDevice::MemoryBlockDevice(VirtualAddress address, size_t size) : m_address(address), m_size(size) {}

Result<void> MemoryBlockDevice::read(size_t offset, size_t size, uint8_t *buffer) {
    auto source = m_address.as_ptr();
    if (offset < 0 || (offset + size) >= m_size) {
        return Error::from_code(1);
    }
    memcpy(buffer, source + offset, size);
    return {};
}

Result<void> MemoryBlockDevice::write(size_t offset, size_t size, uint8_t *buffer) {
    auto destination = m_address.as_ptr() + offset;
    if (offset < 0 || (offset + size) >= m_size) {
        return Error::from_code(1);
    }
    memcpy(destination, buffer, size);
    return {};
}
