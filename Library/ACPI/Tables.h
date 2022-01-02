// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

#define RSDP_SIGNATURE 0x2052545020445352
struct PACKED RootSystemDescriptionPointer {
    uint64_t signature;
    uint8_t checksum;
    uint8_t oemId[6];
    uint8_t revision;
    uint32_t rsdtAddress;
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[8];
};
