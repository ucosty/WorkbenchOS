// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Types.h>

class PhysicalAddress {
public:
    explicit PhysicalAddress(uint64_t address) : m_address(address) {}
    [[nodiscard]] uint64_t as_address() const { return m_address; }
    static PhysicalAddress from_base_and_page_offset(uint64_t base_address, uint64_t page_offset) {
        return PhysicalAddress(base_address + (page_offset * Page));
    }
private:
    uint64_t m_address;
};
