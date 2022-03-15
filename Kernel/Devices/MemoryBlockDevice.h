// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Devices/BlockDevice.h>
#include <VirtualAddress.h>

class MemoryBlockDevice : public BlockDevice {
public:
    MemoryBlockDevice(VirtualAddress address, size_t size);

    Result<void> read(size_t offset, size_t size, uint8_t *buffer) override;

    Result<void> write(size_t offset, size_t size, uint8_t *buffer) override;

private:
    VirtualAddress m_address;
    size_t m_size;
};
