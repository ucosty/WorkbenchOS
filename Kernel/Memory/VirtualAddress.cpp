// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "VirtualAddress.h"

namespace Kernel {
bool operator>(const VirtualAddress &lhs, const VirtualAddress &rhs) {
    return rhs.m_address < lhs.m_address;
}
bool operator>(const VirtualAddress &lhs, const uint64_t rhs) {
    return lhs.m_address > rhs;
}
size_t operator-(const VirtualAddress &lhs, const VirtualAddress &rhs) {
    return lhs.m_address - rhs.m_address;
}
size_t operator-(const VirtualAddress &lhs, const uint64_t rhs) {
    return lhs.m_address - rhs;
}
size_t operator-(const uint64_t lhs, const VirtualAddress &rhs) {
    return lhs - rhs.m_address;
}
uint8_t *VirtualAddress::as_ptr() const { return reinterpret_cast<uint8_t *>(m_address); }
size_t VirtualAddress::difference(VirtualAddress other) const {
    return other > m_address ? other - m_address : m_address - other;
}
VirtualAddress VirtualAddress::offset(uint64_t offset) const {
    return VirtualAddress(m_address + offset);
}
VirtualAddress VirtualAddress::from_base_and_page_offset(uint64_t base_address, uint64_t page_offset) {
    return VirtualAddress(base_address + (page_offset * Page));
}
}// namespace Kernel