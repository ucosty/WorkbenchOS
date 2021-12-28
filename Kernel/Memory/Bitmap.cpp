// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Bitmap.h"

namespace Kernel {
void Bitmap::init(size_t block_size, size_t memory_size, uint64_t base_address, uint64_t storage_address) {
    m_block_size = block_size;
    m_base_address = base_address;
    m_total_blocks = memory_size / block_size;
    m_free_blocks = m_total_blocks;
    m_storage_count = m_total_blocks / 32;
    m_storage_unit_physical_size = block_size * 32;
    m_storage = reinterpret_cast<uint64_t *>(storage_address);
    for (int i = 0; i < m_storage_count; i++) {
        m_storage[i] = 0;
    }
}

Result<PhysicalAddress> Bitmap::allocate() {
    if (m_free_blocks == 0)
        return Lib::Error::from_code(1);

    for (int i = 0; i < m_storage_count; i++) {
        if (m_storage[i] != BUDDY_FULL) {
            auto block = i * blocks_per_storage_unit;
            auto offset = find_free(m_storage[i]);
            set_allocated({.block = block, .offset = offset});
            m_free_blocks--;
            return PhysicalAddress(m_base_address + (block * m_storage_unit_physical_size) + (offset * m_block_size));
        }
    }
    return Lib::Error::from_code(1);
}

Result<void> Bitmap::free(PhysicalAddress address) {
    auto block_and_offset = address_to_block_and_offset(address.as_address());
    uint64_t mask = ~(3 << (block_and_offset.offset * 2));
    m_storage[block_and_offset.block] &= mask;
    m_free_blocks++;
    return {};
}

void Bitmap::set_allocated(PhysicalAddress address) {
    auto block_and_offset = address_to_block_and_offset(address.as_address());
    set_allocated(block_and_offset);
}

void Bitmap::set_allocated(BlockAndOffset block_and_offset) {
    uint64_t mask = 3 << (block_and_offset.offset * 2);
    m_storage[block_and_offset.block] |= mask;
}

BlockAndOffset Bitmap::address_to_block_and_offset(uint64_t address) const {
    auto address_adjusted = address - m_base_address;
    return BlockAndOffset{
        .block = address_adjusted / m_storage_unit_physical_size,
        .offset = (address_adjusted % m_storage_unit_physical_size) / m_block_size};
}

size_t Bitmap::find_free(uint64_t &bitmap) {
    for (int i = 0; i < 32; i++) {
        uint64_t mask = 0b11ull << (i * 2);
        if ((bitmap & mask) == 0) {
            return i;
        }
    }
    return -1;
}

uint64_t *Bitmap::get_bitmap_block(size_t block_index, uint64_t *storage_offset) {
    auto storage_index = block_index / blocks_per_storage_unit;
    *storage_offset = block_index - (storage_index * blocks_per_storage_unit);
    return &m_storage[storage_index];
}
}// namespace Kernel
