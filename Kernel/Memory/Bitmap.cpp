// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/CString.h>
#include <LibStd/Try.h>
#include <Memory/Bitmap.h>

using namespace Std;

namespace Kernel {
void Bitmap::init(size_t page_size, size_t ram_size, u64 base_address, u64 storage_address) {
    m_page_size = page_size;
    m_base_address = base_address;
    m_total_pages = ram_size / page_size;
    m_free_pages = m_total_pages;
    m_block_count = m_total_pages / pages_per_block;
    m_block_full_size = page_size * pages_per_block;
    m_storage = reinterpret_cast<u64 *>(storage_address);
    for (int i = 0; i < m_block_count; i++) {
        m_storage[i] = 0;
    }
}

Result<PhysicalAddress> Bitmap::allocate() {
    if (m_free_pages == 0)
        return Error::from_code(1);

    for (u64 i = 0; i < m_block_count; i++) {
        if (m_storage[i] != BITMAP_FULL) {
            auto offset = TRY(find_free(m_storage[i]));
            set_allocated({.block = i, .offset = offset});
            m_free_pages--;
            auto address = PhysicalAddress(m_base_address + (i * m_block_full_size) + (offset * m_page_size));
            memset((char *) address.as_ptr(), 0, Page);
            return address;
        }
    }
    return Error::from_code(1);
}

Result<void> Bitmap::free(PhysicalAddress address) {
    auto block_and_offset = address_to_block_and_offset(address.as_address());
    u64 mask = ~(1ull << block_and_offset.offset);
    m_storage[block_and_offset.block] &= mask;
    m_free_pages++;
    return {};
}

void Bitmap::set_allocated(PhysicalAddress address) {
    auto block_and_offset = address_to_block_and_offset(address.as_address());
    set_allocated(block_and_offset);
}

void Bitmap::set_allocated(BlockAndOffset block_and_offset) {
    u64 mask = 1ull << block_and_offset.offset;
    m_storage[block_and_offset.block] |= mask;
}

bool Bitmap::is_allocated(PhysicalAddress address) {
    auto block_and_offset = address_to_block_and_offset(address.as_address());
    u64 mask = 1ull << block_and_offset.offset;
    return m_storage[block_and_offset.block] & mask;
}

BlockAndOffset Bitmap::address_to_block_and_offset(u64 address) const {
    auto address_adjusted = address - m_base_address;
    return BlockAndOffset{
        .block = address_adjusted / m_block_full_size,
        .offset = (address_adjusted % m_block_full_size) / m_page_size};
}

Result<size_t> Bitmap::find_free(u64 bitmap) {
    for (int i = 0; i < 64; i++) {
        u64 mask = 1ull << i;
        if ((bitmap & mask) == 0) {
            return i;
        }
    }
    return Error::from_code(1);
}
}// namespace Kernel
