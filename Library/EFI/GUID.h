// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "LibStd/Types.h"

namespace EFI {

typedef struct GUID {
    union {
        alignas(8) uint8_t u8[16];
        alignas(8) uint16_t u16[8];
        alignas(8) uint32_t u32[4];
        alignas(8) uint64_t u64[2];
        struct {
            alignas(8) uint32_t ms1;
            uint16_t ms2;
            uint16_t ms3;
            uint8_t ms4[8];
        };
    };
    constexpr bool equals(const GUID &other) {
        return ms1 == other.ms1
               && ms2 == other.ms2
               && ms3 == other.ms3
               && ms4[0] == other.ms4[0]
               && ms4[1] == other.ms4[1]
               && ms4[2] == other.ms4[2]
               && ms4[3] == other.ms4[3]
               && ms4[4] == other.ms4[4]
               && ms4[5] == other.ms4[5]
               && ms4[6] == other.ms4[6]
               && ms4[7] == other.ms4[7];
    }
} GUID;

static_assert(sizeof(GUID) == 16);

}// namespace EFI