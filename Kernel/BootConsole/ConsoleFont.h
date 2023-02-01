// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>

struct Glyph {
    u16 row[16];
    u8 width;
};

Glyph font_basic[] = {
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // NUL, 0
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // SOH, 1
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // STX, 2
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // ETX, 3
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // EOT, 4
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // ENQ, 5
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // ACK, 6
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // BEL, 7

    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  BS, 8
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // TAB, 9
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  LF, A
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  VT, B
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  FF, C
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  CR, D
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  SO, E
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  SI, F

    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // DLE, 10
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // DC1, 11
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // DC2, 12
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // DC3, 13
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // DC4, 14
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // NAK, 15
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // SYN, 16
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // ETB, 17

    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // CAN, 18
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  EM, 19
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // SUB, 1A
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // ESC, 1B
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  FS, 1C
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  GS, 1D
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  RS, 1E
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, //  US, 1F

    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 4}, // SPC, 20
    {{ 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00 }, 4}, // !, 21
    {{ 0x0, 0x0, 0x0, 0xA, 0xA, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, 5}, // ", 22
    {{ 0x00, 0x00, 0x00, 0x48, 0x48, 0xFE, 0x24, 0x24, 0x7F, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 }, 8}, // #, 23

    {{ 0x00, 0x08, 0x1C, 0x2A, 0x0A, 0x0E, 0x1C, 0x38, 0x28, 0x2A, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00 }, 7}, // $, 24

    {{ 0x0, 0x0, 0x0, 0x1EC, 0x92, 0x92, 0x4C, 0x40, 0x1A0, 0x250, 0x250, 0x188, 0x0, 0x0, 0x0, 0x0 }, 11}, // %, 25
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // &, 26
    {{ 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, 3}, // ', 27

    {{ 0x0, 0x0, 0x8, 0x4, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x4, 0x8, 0x0, 0x0, 0x0 }, 5}, // (, 28
    {{ 0x0, 0x0, 0x2, 0x4, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0x4, 0x2, 0x0, 0x0, 0x0  }, 5}, // ), 29
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0x2A, 0x1C, 0x2A, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0,  }, 7}, // *, 2A
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0x8, 0x3E, 0x8, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0 }, 7}, // +, 2B
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x4, 0x2, 0x0, 0x0 }, 4}, // ,, 2C
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7E, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, 8}, // -, 2D
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x0, 0x0, 0x0, 0x0,  }, 4}, // ., 2E
    {{ 0x0, 0x0, 0x20, 0x20, 0x10, 0x10, 0x8, 0x8, 0x4, 0x4, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0 }, 7}, // /, 2F

    {{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x76, 0x6E, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // 0, 30
    {{ 0x00, 0x00, 0x00, 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00 }, 5}, // 1, 31
    {{ 0x00, 0x00, 0x00, 0x3C, 0x62, 0x60, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x7E, 0x00, 0x00, 0x00, 0x00 }, 8}, // 2, 32
    {{ 0x00, 0x00, 0x00, 0x7E, 0x30, 0x18, 0x3C, 0x60, 0x60, 0x60, 0x62, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // 3, 33
    {{ 0x00, 0x00, 0x00, 0x60, 0x70, 0x68, 0x64, 0x62, 0xFE, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00 }, 9}, // 4, 34
    {{ 0x00, 0x00, 0x00, 0x7E, 0x06, 0x06, 0x3E, 0x60, 0x60, 0x60, 0x62, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // 5, 35
    {{ 0x00, 0x00, 0x00, 0x38, 0x0C, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // 6, 36
    {{ 0x00, 0x00, 0x00, 0x7E, 0x60, 0x60, 0x60, 0x30, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00 }, 8}, // 7, 37

    {{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // 8, 38
    {{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x30, 0x1C, 0x00, 0x00, 0x00, 0x00 }, 8}, // 9, 39
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x0, 0x0, 0x0, 0x6, 0x6, 0x0, 0x0, 0x0, 0x0 }, 4}, // :, 3A
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x0, 0x0, 0x0, 0x6, 0x6, 0x4, 0x2, 0x0, 0x0 }, 4}, // ;, 3B
    {{ 0x0, 0x0, 0x0, 0x0, 0x30, 0x18, 0xC, 0x6, 0xC, 0x18, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0 }, 7}, // <, 3C
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 8}, // =, 3D
    {{ 0x0, 0x0, 0x0, 0x0, 0x6, 0xC, 0x18, 0x30, 0x18, 0xC, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0 }, 7}, // >, 3E
    {{ 0x0, 0x0, 0x0, 0x3C, 0x62, 0x60, 0x30, 0x18, 0x18, 0x0, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0 }, 8}, // ?, 3F

    {{ 0x00, 0x00, 0x00, 0xF8, 0x104, 0x272, 0x24A, 0x24A, 0x24A, 0x1B2, 0x4, 0xF8, 0x00, 0x00, 0x00, 0x00 }, 11}, // @, 40
    {{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00 }, 8}, // A, 41
    {{ 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00 }, 8}, // B, 42
    {{ 0x00, 0x00, 0x00, 0x3C, 0x46, 0x06, 0x06, 0x06, 0x06, 0x06, 0x46, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // C, 43
    {{ 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00 }, 8}, // D, 44
    {{ 0x00, 0x00, 0x00, 0x3E, 0x06, 0x06, 0x06, 0x1E, 0x06, 0x06, 0x06, 0x3E, 0x00, 0x00, 0x00, 0x00 }, 8}, // E, 45
    {{ 0x00, 0x00, 0x00, 0x3E, 0x06, 0x06, 0x06, 0x1E, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00 }, 7}, // F, 46
    {{ 0x00, 0x00, 0x00, 0x3C, 0x46, 0x06, 0x06, 0x76, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // G, 47

    {{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00 }, 8}, // H, 48
    {{ 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00 }, 4}, // I, 49
    {{ 0x00, 0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // J, 4A
    {{ 0x00, 0x00, 0x00, 0xC6, 0x66, 0x36, 0x1E, 0x0E, 0x1E, 0x36, 0x66, 0xC6, 0x00, 0x00, 0x00, 0x00 }, 9}, // K, 4B
    {{ 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3E, 0x00, 0x00, 0x00, 0x00 }, 7}, // L, 4C
    {{ 0x00, 0x00, 0x00, 0x602, 0x706, 0x78E, 0x7DE, 0x6FA, 0x672, 0x622, 0x602, 0x602, 0x00, 0x00, 0x00, 0x00 }, 12}, // M, 4D
    {{ 0x00, 0x00, 0x00, 0x82, 0x86, 0x8E, 0x9E, 0xBA, 0xF2, 0xE2, 0xC2, 0x82, 0x00, 0x00, 0x00, 0x00 }, 9}, // N, 4E
    {{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // O, 4F

    {{ 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00 }, 8}, // P, 50
    {{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x60, 0x00, 0x00, 0x00 }, 8}, // Q, 51
    {{ 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00 }, 8}, // R, 52
    {{ 0x00, 0x00, 0x00, 0x1C, 0x26, 0x06, 0x0E, 0x1C, 0x38, 0x30, 0x32, 0x1C, 0x00, 0x00, 0x00, 0x00 }, 6}, // S, 53

    {{ 0x00, 0x00, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00 }, 8}, // T, 54
    {{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00 }, 8}, // U, 55
    {{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x26, 0x1E, 0x00, 0x00, 0x00, 0x00 }, 8}, // V, 56
    {{ 0x00, 0x00, 0x00, 0x666, 0x666, 0x666, 0x666, 0x666, 0x666, 0x666, 0x266, 0x1FE, 0x00, 0x00, 0x00, 0x00 }, 12}, // W, 57

    {{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00 }, 8}, // X, 58
    {{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00 }, 8}, // Y, 59
    {{ 0x00, 0x00, 0x00, 0x7E, 0x60, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x06, 0x7E, 0x00, 0x00, 0x00, 0x00 }, 8}, // Z, 5A
    {{ 0x0, 0x0, 0xE, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0xE, 0x0, 0x0, 0x0,  }, 5}, // [, 5B
    {{ 0x0, 0x0, 0x2, 0x2, 0x4, 0x4, 0x8, 0x8, 0x10, 0x10, 0x20, 0x20, 0x0, 0x0, 0x0, 0x0 }, 7}, // \, 5C
    {{ 0x0, 0x0, 0xE, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 0xE, 0x0, 0x0, 0x0 }, 5}, // ], 5D
    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // ^, 5E
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0 }, 8}, // _, 5F

    {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0}, // `, 60
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x3C, 0x62, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x0, 0x0, 0x0, 0x0 }, 8}, // a, 61
    {{ 0x0, 0x0, 0x0, 0x6, 0x6, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x0, 0x0, 0x0, 0x0 }, 8}, // b, 62
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x1C, 0x26, 0x6, 0x6, 0x6, 0x26, 0x1C, 0x0, 0x0, 0x0, 0x0 }, 7}, //c,63
    {{ 0x0, 0x0, 0x0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x0, 0x0, 0x0, 0x0 }, 8}, //d
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x7E, 0x6, 0x46, 0x3C, 0x0, 0x0, 0x0, 0x0 }, 8}, //e
    {{ 0x0, 0x0, 0x0, 0x0, 0x38, 0xC, 0x1E, 0xC, 0xC, 0xC, 0xC, 0xC, 0x0, 0x0, 0x0, 0x0 }, 7}, //f
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x62, 0x3C, 0x0 }, 8}, //g
    {{ 0x0, 0x0, 0x0, 0x6, 0x6, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x0 }, 8}, //h
    {{ 0x0, 0x0, 0x0, 0x6, 0x0, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x0, 0x0, 0x0, 0x0 }, 4}, //i
    {{ 0x0, 0x0, 0x0, 0x30, 0x0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x1C, 0x0, 0x0, 0x0 }, 7}, //j
    {{ 0x0, 0x0, 0x0, 0x6, 0x6, 0x66, 0x36, 0x1E, 0xE, 0x1E, 0x36, 0x66, 0x0, 0x0, 0x0, 0x0 }, 8}, //k
    {{ 0x0, 0x0, 0x0, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x0, 0x0, 0x0, 0x0 }, 4}, //l
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x3BE, 0x666, 0x666, 0x666, 0x666, 0x666, 0x666, 0x0, 0x0, 0x0, 0x0 }, 12}, //m
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x0 }, 8}, //n
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0, 0x0, 0x0, 0x0 }, 8}, //o
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x6, 0x6, 0x0, 0x0 }, 8}, //p
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x0, 0x0 }, 8}, //q

    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x36, 0x3E, 0x6, 0x6, 0x6, 0x6, 0x6, 0x0, 0x0, 0x0, 0x0 }, 7}, //r
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x1C, 0x26, 0xE, 0x1C, 0x38, 0x32, 0x1C, 0x0, 0x0, 0x0, 0x0 }, 7}, //s
    {{ 0x0, 0x0, 0x0, 0xC, 0xC, 0x1E, 0xC, 0xC, 0xC, 0xC, 0xC, 0x18, 0x0, 0x0, 0x0, 0x0 }, 6}, //t
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x0, 0x0, 0x0, 0x0 }, 8}, //u
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x26, 0x1E, 0x0, 0x0, 0x0, 0x0 }, 8}, //v
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x666, 0x666, 0x666, 0x666, 0x666, 0x266, 0x1FE, 0x0, 0x0, 0x0, 0x0  }, 12}, //w
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x0, 0x0, 0x0, 0x0 }, 8}, //x
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x62, 0x3C, 0x0 }, 8}, //y
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x7E, 0x60, 0x30, 0x18, 0xC, 0x6, 0x7E, 0x0, 0x0, 0x0, 0x0 }, 8}, //z

    {{ 0x0, 0x0, 0x8, 0x4, 0x4, 0x4, 0x4, 0x2, 0x4, 0x4, 0x4, 0x4, 0x8, 0x0, 0x0, 0x0 }, 5}, // {
    {{ 0x0, 0x0, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0 }, 3}, // |
    {{ 0x0, 0x0, 0x2, 0x4, 0x4, 0x4, 0x4, 0x8, 0x4, 0x4, 0x4, 0x4, 0x2, 0x0, 0x0, 0x0 }, 5}, // }
    {{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4C, 0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, 8}, // ~
};
