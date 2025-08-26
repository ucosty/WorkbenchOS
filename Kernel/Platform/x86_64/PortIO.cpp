// WorkbenchOS
// Copyright (c) 2025 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "PortIO.h"

namespace PortIO {
    void out8(uint16_t p, uint8_t v) {
        asm volatile("outb %0, %1"
            :
            : "a"(v), "Nd"(p));
    }

    void out16(uint16_t p, uint16_t v) {
        asm volatile("outw %0, %1"
            :
            : "a"(v), "Nd"(p));
    }

    void out32(uint16_t p, uint32_t v) {
        asm volatile("out %0, %1"
            :
            : "a"(v), "Nd"(p));
    }

    uint8_t in8(uint16_t p) {
        u8 val;
        asm volatile("inb %1, %0"
            : "=q"(val)
            : "Nd"(p));
        return val;
    }

    uint16_t in16(uint16_t p) {
        u16 val;
        asm volatile("inw %1, %0"
            : "=q"(val)
            : "Nd"(p));
        return val;
    }

    uint32_t in32(uint16_t p) {
        u32 val;
        asm volatile("in %1, %0"
            : "=q"(val)
            : "Nd"(p));
        return val;
    }
}
