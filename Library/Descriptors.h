// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

#define SEGMENT_READ_WRITE 2
#define SEGMENT_EXECUTE 8

constexpr uint8_t DESCRIPTOR_SYSTEM = 0;
constexpr uint8_t SEGMENT_RD_RW = 1;

struct SegmentDescriptor {
    uint64_t segment_limit : 16;
    uint64_t base_low : 16;
    uint64_t base : 8;
    uint64_t segment_type : 4;
    uint64_t descriptor_type : 1;
    uint64_t privilege_level : 2;
    uint64_t present : 1;
    uint64_t segment_limit_high : 4;
    uint64_t available : 1;
    uint64_t code_segment_64bit : 1;
    uint64_t default_operation_size : 1;
    uint64_t granularity : 1;
    uint64_t base_high : 8;

    [[nodiscard]] constexpr uint64_t descriptor() const {
        return (uint64_t) segment_limit | (uint64_t) base_low << 16 | (uint64_t) base << 32 | (uint64_t) segment_type << 40 | (uint64_t) descriptor_type << 44 | (uint64_t) privilege_level << 45 | (uint64_t) present << 47 | (uint64_t) segment_limit_high << 48 | (uint64_t) available << 52 | (uint64_t) code_segment_64bit << 53 | (uint64_t) default_operation_size << 54 | (uint64_t) granularity << 55 | (uint64_t) base_high << 56;
    }
};
static_assert(sizeof(SegmentDescriptor) == 8);

struct PACKED InterruptDescriptor {
    uint64_t offset : 16;
    uint64_t segment_selector : 16;
    uint64_t ist : 2;
    uint64_t reserved : 6;
    uint64_t type : 4;
    uint64_t reserved_1 : 1;
    uint64_t descriptor_privilege_level : 2;
    uint64_t present : 1;
    uint64_t offset_2 : 16;
    uint32_t offset_3;
    uint32_t reserved_2;
};
static_assert(sizeof(InterruptDescriptor) == 16);

struct PACKED DescriptorTablePointer {
    uint16_t limit;
    uint64_t address;
};

static_assert(sizeof(SegmentDescriptor) == 8);
static_assert(sizeof(InterruptDescriptor) == 16);
static_assert(sizeof(DescriptorTablePointer) == 10);
