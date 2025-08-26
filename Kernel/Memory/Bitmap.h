// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <PhysicalAddress.h>
#include <LibStd/Result.h>
#include <LibStd/Types.h>

namespace Kernel {
constexpr u64 BITMAP_FULL = 0xffffffffffffffff;

struct BlockAndOffset {
    u64 block;
    u64 offset;
};

enum class BitmapError {
    InitInvalidPageSize,
    InitNoPages,
    NoFreeMemory,
    AllocationFailure,
    Range,
    DoubleFree,
    Full
};

class Bitmap {
public:
    Std::Result<void, BitmapError> init(size_t page_size, size_t ram_size, u64 base_address, u64 storage_address);

    [[nodiscard]] Std::Result<PhysicalAddress, BitmapError> allocate();
    [[nodiscard]] Std::Result<void, BitmapError> free(PhysicalAddress address);
    [[nodiscard]] Std::Result<PhysicalAddress, BitmapError> allocate_contiguous(size_t pages);
    void set_allocated(PhysicalAddress address);
    void set_allocated(BlockAndOffset block_and_offset);
    [[nodiscard]] bool is_allocated(PhysicalAddress address) const;

private:
    static constexpr size_t pages_per_block = 64;
    size_t m_page_size = 0;
    size_t m_total_pages = 0;
    size_t m_free_pages = 0;
    size_t m_block_count = 0;
    size_t m_block_full_size = 0;
    u64 *m_storage = nullptr;
    u64 m_base_address = 0;

    [[nodiscard]] BlockAndOffset address_to_block_and_offset(u64 address) const;
    static Std::Result<size_t, BitmapError> find_free(u64 bitmap);
    static Std::Result<size_t, BitmapError> find_n_free(u64 bitmap, u8 n);
    void set_n_allocated(const BlockAndOffset block_and_offset, u8 n);
};
}// namespace Kernel
