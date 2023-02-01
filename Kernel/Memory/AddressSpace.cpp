// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "AddressSpace.h"

namespace VirtualMemory {
AddressSpace::AddressSpace() {
    auto start = VirtualAddress((u64) 0);
    m_regions = new FreeRegion(nullptr, start, ADDRESS_SPACE_PAGES);
}

Optional<Region *> AddressSpace::find_by_address(VirtualAddress address) {
    // Naive implementation
    auto region = m_regions;
    while (region != nullptr) {
        if (region->in_region(address)) {
            return region;
        }
        region = region->next();
    }
    return {};
}

Result<void> AddressSpace::create_memory_region() {
    auto start = VirtualAddress((u64) 0);
    m_regions = new MemoryRegion(m_regions, start, 0);
    return {};
}

Result<void> AddressSpace::create_file_region() {
    return {};
}

Result<void> AddressSpace::create_stack_region() {
    return {};
}


}// namespace VirtualMemory
