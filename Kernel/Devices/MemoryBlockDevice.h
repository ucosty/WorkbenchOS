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

    Std::Result<void> read(size_t offset, size_t size, u8 *buffer) override;

    Std::Result<void> write(size_t offset, size_t size, u8 *buffer) override;

private:
    VirtualAddress m_address;
    size_t m_size;
};
