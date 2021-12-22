// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

constexpr uint8_t SEGMENT_READ_WRITE = 2;
constexpr uint8_t SEGMENT_EXECUTE = 8;

constexpr uint8_t DESCRIPTOR_SYSTEM = 0;
constexpr uint8_t DESCRIPTOR_CODE_DATA = 1;

struct SegmentDescriptor {
    uint64_t segment_limit : 16;
    uint64_t base_low : 16;
    uint64_t base : 8;
    uint64_t segment_type : 4;
    uint64_t descriptor_type : 1;
    uint64_t privilege_level: 2;
    uint64_t present : 1;
    uint64_t segment_limit_high : 4;
    uint64_t available : 1;
    uint64_t code_segment_64bit : 1;
    uint64_t default_operation_size : 1;
    uint64_t granularity : 1;
    uint64_t base_high : 8;
};

struct PACKED DescriptorTablePointer {
    uint16_t limit;
    uint64_t address;
};

static_assert(sizeof(SegmentDescriptor) == 8);
static_assert(sizeof(DescriptorTablePointer) == 10);
