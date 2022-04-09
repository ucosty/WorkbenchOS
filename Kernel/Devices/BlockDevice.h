// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>
#include <LibStd/Result.h>

class BlockDevice {
public:
    virtual Std::Result<void> read(size_t offset, size_t size, uint8_t *buffer) = 0;

    virtual Std::Result<void> write(size_t offset, size_t size, uint8_t *buffer) = 0;
};
