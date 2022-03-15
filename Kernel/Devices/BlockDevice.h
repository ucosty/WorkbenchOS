// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Types.h>
#include <Result.h>

class BlockDevice {
public:
    virtual Result<void> read(size_t offset, size_t size, uint8_t *buffer) = 0;

    virtual Result<void> write(size_t offset, size_t size, uint8_t *buffer) = 0;
};
