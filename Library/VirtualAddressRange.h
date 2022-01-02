// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <VirtualAddress.h>

class VirtualAddressRange {
public:
    VirtualAddressRange() = default;
    VirtualAddressRange(VirtualAddress start, size_t size) : m_start(start), m_size_in_pages(size) {}
    [[nodiscard]] bool in_range(const VirtualAddress &address) const { return address > m_start && address < m_start.offset_pages(m_size_in_pages); }
private:
    VirtualAddress m_start{nullptr};
    size_t m_size_in_pages{0};
};
