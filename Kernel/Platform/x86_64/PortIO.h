// WorkbenchOS
// Copyright (c) 2025 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>

namespace PortIO {
    void out8(uint16_t p, uint8_t v);
    void out16(uint16_t p, uint16_t v);
    void out32(uint16_t p, uint32_t v);
    uint8_t  in8(uint16_t p);
    uint16_t in16(uint16_t p);
    uint32_t in32(uint16_t p);
}
