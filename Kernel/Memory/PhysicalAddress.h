// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Types.h>

constexpr uint64_t PHYSICAL_MAPPING_BASE = 0xffffff0000000000;

class PhysicalAddress {
public:
    explicit PhysicalAddress(uint64_t address) : m_address(address) {}
    [[nodiscard]] uint64_t as_address() const { return m_address; }
    [[nodiscard]] uint64_t as_mapped_address() const { return m_address + PHYSICAL_MAPPING_BASE; }
    [[nodiscard]] uint8_t *as_ptr() const { return reinterpret_cast<uint8_t *>(as_mapped_address()); }
    static PhysicalAddress from_base_and_page_offset(uint64_t base_address, uint64_t page_offset) {
        return PhysicalAddress(base_address + (page_offset * Page));
    }
    template<typename T>
    T as_ptr() const {
        return reinterpret_cast<T>(as_mapped_address());
    }
private:
    uint64_t m_address;
};
