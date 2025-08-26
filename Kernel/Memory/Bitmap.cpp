// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/CString.h>
#include <LibStd/Try.h>
#include <Memory/Bitmap.h>

using namespace Std;

namespace Kernel {

Result<void, BitmapError> Bitmap::init(const size_t page_size, const size_t ram_size, const u64 base_address, const u64 storage_address) {
    if (page_size == 0 || (page_size & (page_size - 1)) != 0) [[unlikely]] {
        return BitmapError::InitInvalidPageSize;
    }
    if (base_address % page_size) [[unlikely]] {
        return BitmapError::InitNoPages;
    }

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

    return {};
}

Result<PhysicalAddress, BitmapError> Bitmap::allocate() {
    if (m_free_pages == 0)
        return BitmapError::NoFreeMemory;

    for (u64 i = 0; i < m_block_count; i++) {
        if (m_storage[i] != BITMAP_FULL) {
            const auto result = find_free(m_storage[i]);
            if (result.is_error()) continue;
            const auto offset = result.get();
            set_allocated({.block = i, .offset = offset});
            const auto address = PhysicalAddress(m_base_address + (i * m_block_full_size) + (offset * m_page_size));
            memset(reinterpret_cast<char *>(address.as_ptr()), 0, m_page_size);
            return address;
        }
    }

    return BitmapError::AllocationFailure;
}

Result<void, BitmapError> Bitmap::free(const PhysicalAddress address) {
    if (!is_allocated(address)) {
        return BitmapError::DoubleFree;
    }

    auto [block, offset] = address_to_block_and_offset(address.as_address());
    const u64 mask = ~(1ull << offset);
    m_storage[block] &= mask;
    m_free_pages++;
    return {};
}

Result<PhysicalAddress, BitmapError> Bitmap::allocate_contiguous(size_t pages) {
    if (m_free_pages == 0)
        return BitmapError::NoFreeMemory;

    for (u64 i = 0; i < m_block_count; i++) {

        const auto result = find_n_free(m_storage[i], pages);
        if (result.is_error()) continue;

        const auto offset = result.get();
        set_n_allocated({.block = i, .offset = offset}, pages);

        const auto address = PhysicalAddress(m_base_address + (i * m_block_full_size) + (offset * m_page_size));
        memset(reinterpret_cast<char *>(address.as_ptr()), 0, m_page_size);
        return address;
    }
    return BitmapError::NoFreeMemory;
}

void Bitmap::set_allocated(const PhysicalAddress address) {
    const auto block_and_offset = address_to_block_and_offset(address.as_address());
    set_allocated(block_and_offset);
}

void Bitmap::set_allocated(const BlockAndOffset block_and_offset) {
    const u64 mask = 1ull << block_and_offset.offset;
    m_storage[block_and_offset.block] |= mask;
    m_free_pages--;
}

bool Bitmap::is_allocated(const PhysicalAddress address) const {
    auto [block, offset] = address_to_block_and_offset(address.as_address());
    const u64 mask = 1ull << offset;
    return (m_storage[block] & mask) != 0;
}

BlockAndOffset Bitmap::address_to_block_and_offset(const u64 address) const {
    const auto address_adjusted = address - m_base_address;
    return BlockAndOffset{
        .block = address_adjusted / m_block_full_size,
        .offset = (address_adjusted % m_block_full_size) / m_page_size
    };
}

Result<size_t, BitmapError> Bitmap::find_free(const u64 bitmap) {
    for (size_t i = 0; i < 64; i++) {
        if (const u64 mask = 1ull << i; (bitmap & mask) == 0) {
            return i;
        }
    }
    return BitmapError::NoFreeMemory;
}

inline u64 make_mask(const int n) {
    if (n >= 64) {
        return ~0LL;
    }

    return (1LL << n) - 1;
}

Result<size_t, BitmapError> Bitmap::find_n_free(const u64 bitmap, const u8 n) {
    auto mask = make_mask(n);
    const auto iterations = 64 - n + 1;
    for (size_t i = 0; i < iterations; i++) {
        if ((mask & bitmap) == 0) {
            return i;
        }
        mask <<= 1;
    }
    return BitmapError::NoFreeMemory;
}

void Bitmap::set_n_allocated(const BlockAndOffset block_and_offset, u8 n) {
    auto mask = make_mask(n);
    mask <<= block_and_offset.offset;

    m_storage[block_and_offset.block] |= mask;
    m_free_pages-= n;
}
}// namespace Kernel
