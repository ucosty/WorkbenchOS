// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ConsoleIO.h"
#include "UnbufferedConsole.h"
#include <Devices/MemoryBlockDevice.h>
#include <LibStd/CString.h>

using namespace Std;

MemoryBlockDevice::MemoryBlockDevice(VirtualAddress address, size_t size) : m_address(address), m_size(size) {}

void hexdump(const u8 *buffer, size_t size);

void *copy(void *destination, const void *source, size_t num) {
    if (destination == nullptr || source == nullptr) return nullptr;
    auto source_address = reinterpret_cast<size_t>(source);
    auto destination_address = reinterpret_cast<size_t>(destination);

    // Perfect alignment
    if (source_address % 8 == 0 && destination_address % 8 == 0 && num % 8 == 0) {
        auto *src = reinterpret_cast<const u64 *>(source);
        auto *dst = reinterpret_cast<u64 *>(destination);
        auto quad_words = num / sizeof(u64);

        for (int i = 0; i < quad_words; i++) {
            dst[i] = src[i];
        }

        return destination;
    }

    if (source_address % 4 == 0 && destination_address % 4 == 0 && num % 4 == 0) {
        auto *src = reinterpret_cast<const u32 *>(source);
        auto *dst = reinterpret_cast<u32 *>(destination);
        auto quad_words = num / sizeof(u32);

        for (int i = 0; i < quad_words; i++) {
            dst[i] = src[i];
        }

        return destination;
    }

    // Not perfect alignment
    char *dst = (char *) destination;
    char *src = (char *) source;
    for (size_t i = 0; i < num; i++) {
        dst[i] = src[i];
    }
    return destination;
}

Result<void> MemoryBlockDevice::read(size_t offset, size_t size, u8 *buffer) {
    auto source = m_address.as_ptr();
    if (offset + size > m_size) {
        return Error::from_code(1);
    }
    copy(buffer, source + offset, size);
    return {};
}

Result<void> MemoryBlockDevice::write(size_t offset, size_t size, u8 *buffer) {
    auto destination = m_address.as_ptr() + offset;
    if (offset + size >= m_size) {
        return Error::from_code(1);
    }
    memcpy(destination, buffer, size);
    return {};
}
