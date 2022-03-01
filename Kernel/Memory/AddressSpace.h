// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Region.h"
#include <Optional.h>
#include <Result.h>

namespace VirtualMemory {

const uint64_t ADDRESS_SPACE_PAGES = 0x800000000;

class AddressSpace {
public:
    AddressSpace();
    Optional<Region *> find_by_address(VirtualAddress address);

private:
    Result<void> create_memory_region();
    Result<void> create_file_region();
    Result<void> create_stack_region();
    Region *m_regions{nullptr};
};
}// namespace VirtualMemory
