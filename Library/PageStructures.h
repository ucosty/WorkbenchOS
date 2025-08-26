// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by matthew on 21/12/2021.
//

#pragma once

#include "LibStd/Types.h"
#include <PhysicalAddress.h>

struct PACKED PML4Entry {
    uint64_t present : 1;
    uint64_t writeable : 1;
    uint64_t user_access : 1;
    uint64_t write_through : 1;
    uint64_t cache_disabled : 1;
    uint64_t accessed : 1;
    uint64_t ignored_3 : 1;
    uint64_t size : 1;
    uint64_t ignored_2 : 4;
    uint64_t physical_address : 28;
    uint64_t reserved_1 : 12;
    uint64_t ignored_1 : 11;
    uint64_t execution_disabled : 1;

    [[nodiscard]] PhysicalAddress get_physical_address() const {
        return PhysicalAddress((uint64_t)physical_address << 12);
    }
};

struct PACKED PageDirectoryPointerTableEntry {
    uint64_t present : 1;
    uint64_t writeable : 1;
    uint64_t user_access : 1;
    uint64_t write_through : 1;
    uint64_t cache_disabled : 1;
    uint64_t accessed : 1;
    uint64_t ignored_3 : 1;
    uint64_t size : 1;
    uint64_t global : 1;
    uint64_t ignored_2 : 3;
    uint64_t physical_address : 28;
    uint64_t reserved_1 : 12;
    uint64_t ignored_1 : 11;
    uint64_t execution_disabled : 1;

    [[nodiscard]] PhysicalAddress get_physical_address() const {
        return PhysicalAddress((uint64_t)physical_address << 12);
    }
};

struct PACKED PageDirectoryEntry {
    uint64_t present : 1;
    uint64_t writeable : 1;
    uint64_t user_access : 1;
    uint64_t write_through : 1;
    uint64_t cache_disabled : 1;
    uint64_t accessed : 1;
    uint64_t ignored_3 : 1;
    uint64_t size : 1;
    uint64_t global : 1;
    uint64_t ignored_2 : 3;
    uint64_t physical_address : 28;
    uint64_t reserved_1 : 12;
    uint64_t ignored_1 : 11;
    uint64_t execution_disabled : 1;

    [[nodiscard]] PhysicalAddress get_physical_address() const {
        return PhysicalAddress((uint64_t)physical_address << 12);
    }
};

struct PACKED PageTableEntry {
    uint64_t present : 1;
    uint64_t writeable : 1;
    uint64_t user_access : 1;
    uint64_t write_through : 1;
    uint64_t cache_disabled : 1;
    uint64_t accessed : 1;
    uint64_t dirty : 1;
    uint64_t size : 1;
    uint64_t global : 1;
    uint64_t ignored_2 : 3;
    uint64_t physical_address : 40;
    uint64_t ignored_1 : 11;
    uint64_t execution_disabled : 1;

    [[nodiscard]] PhysicalAddress get_physical_address() const {
        return PhysicalAddress((uint64_t)physical_address << 12);
    }
};

static_assert(sizeof(PML4Entry) == 8);
static_assert(sizeof(PageDirectoryPointerTableEntry) == 8);
static_assert(sizeof(PageDirectoryEntry) == 8);
static_assert(sizeof(PageTableEntry) == 8);
