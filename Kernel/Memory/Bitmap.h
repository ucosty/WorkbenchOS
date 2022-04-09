// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <PhysicalAddress.h>
#include <LibStd/Result.h>
#include <LibStd/Types.h>

namespace Kernel {
constexpr uint64_t BITMAP_FULL = 0xffffffffffffffff;

struct BlockAndOffset {
    uint64_t block;
    uint64_t offset;
};

class Bitmap {
public:
    void init(size_t page_size, size_t ram_size, uint64_t base_address, uint64_t storage);

    [[nodiscard]] Std::Result<PhysicalAddress> allocate();
    [[nodiscard]] Std::Result<void> free(PhysicalAddress address);
    void set_allocated(PhysicalAddress address);
    void set_allocated(BlockAndOffset block_and_offset);
    bool is_allocated(PhysicalAddress address);

private:
    static constexpr size_t pages_per_block = 64;
    size_t m_page_size;
    size_t m_total_pages;
    size_t m_free_pages;
    size_t m_block_count;
    size_t m_block_full_size;
    uint64_t *m_storage;
    uint64_t m_base_address;

    [[nodiscard]] BlockAndOffset address_to_block_and_offset(uint64_t address) const;
    static Std::Result<size_t> find_free(uint64_t bitmap);
};
}// namespace Kernel
