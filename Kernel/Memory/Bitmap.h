// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "PhysicalAddress.h"
#include <Result.h>
#include <Types.h>

namespace Kernel {
constexpr uint64_t BUDDY_FULL = 0xffffffffffffffff;

struct BlockAndOffset {
    uint64_t block;
    uint64_t offset;
};

class Bitmap {
public:
    void init(size_t block_size, size_t memory_size, uint64_t base_address, uint64_t storage);

    [[nodiscard]] Result<PhysicalAddress> allocate();
    [[nodiscard]] Result<void> free(PhysicalAddress address);
    void set_allocated(PhysicalAddress address);
    void set_allocated(BlockAndOffset block_and_offset);
    bool is_allocated(PhysicalAddress address);

private:
    static constexpr size_t blocks_per_storage_unit = 32;
    size_t m_block_size;
    size_t m_total_blocks;
    size_t m_free_blocks;
    size_t m_storage_count;
    size_t m_storage_unit_physical_size;
    uint64_t *m_storage;
    uint64_t m_base_address;

    [[nodiscard]] BlockAndOffset address_to_block_and_offset(uint64_t address) const;
    static Result<size_t> find_free(uint64_t &bitmap);
};
}// namespace Kernel
