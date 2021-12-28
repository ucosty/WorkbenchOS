// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

namespace Kernel {
class VirtualAddress {
public:
    VirtualAddress() = default;
    explicit VirtualAddress(uint64_t address) : m_address(address) {}
    [[nodiscard]] uint64_t as_address() const { return m_address; }
    static VirtualAddress from_base_and_page_offset(uint64_t base_address, uint64_t page_offset);
    [[nodiscard]] VirtualAddress offset(uint64_t offset) const;
    [[nodiscard]] size_t difference(VirtualAddress other) const;
    [[nodiscard]] uint8_t *as_ptr() const;
    friend bool operator>(const VirtualAddress &lhs, const VirtualAddress &rhs);
    friend bool operator>(const VirtualAddress &lhs, uint64_t rhs);
    friend size_t operator-(const VirtualAddress &lhs, const VirtualAddress &rhs);
    friend size_t operator-(const VirtualAddress &lhs, uint64_t rhs);
    friend size_t operator-(uint64_t lhs, const VirtualAddress &rhs);
private:
    uint64_t m_address{0};
};
}// namespace Kernel
