// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "SlabAllocator.h"
#include "LibStd/Result.h"
#include "LibStd/Types.h"
#include <VirtualAddress.h>
#include <VirtualAddressRange.h>

#define ALLOC_BLOCK_SIZE 8

namespace Kernel {
class AllocatedBlock {
public:
    AllocatedBlock(size_t data_size) : m_data_size(data_size) {}

private:
    friend class KmallocSubHeap;

    u64 m_canary{0xd1d1d1d1d1d1d1d1};
    size_t m_data_size;
};

class FreeBlock {
public:
    FreeBlock(const size_t size) : m_block_size(size) {}

    [[nodiscard]] FreeBlock *next() const { return m_next; };

    [[nodiscard]] size_t allocatable_size() const { return m_block_size - sizeof(AllocatedBlock); }

    void coalesce_with_next_block();

    void coalesce_with_previous_block() const;

    [[nodiscard]] size_t block_size() const {
        return m_block_size;
    }

    [[nodiscard]] bool is_canary_valid() const {
        return m_canary == 0xfefefefefefefefe;
    }

    bool can_coalesce_next();

    [[nodiscard]] bool can_coalesce_previous() const;

    void split_if_required(size_t allocation_request);

private:
    friend class KmallocSubHeap;

    u64 m_canary{0xfefefefefefefefe};
    u64 m_block_size;
    FreeBlock *m_previous{nullptr};
    FreeBlock *m_next{nullptr};
};


class KmallocSubHeap {
public:
    Std::Result<void> initialise();

    Std::Result<void> initialise(KmallocSubHeap *next);

    Std::Result<VirtualAddress> allocate(size_t _size);

    Std::Result<void> free(VirtualAddress address);

    void insert_into_free_list(FreeBlock *free_block_to_insert);

    void remove_from_free_list(const FreeBlock *free_block);

    [[nodiscard]] bool can_allocate(const size_t size) const { return m_available >= size; }

    [[nodiscard]] KmallocSubHeap *next() const { return m_next; }

    bool contains_allocation(VirtualAddress address);

private:
    u8 *m_storage{nullptr};
    FreeBlock *m_free_list{nullptr};
    KmallocSubHeap *m_previous{nullptr};
    KmallocSubHeap *m_next{nullptr};
    size_t m_capacity{0x1000000};
    size_t m_available{0x1000000};
};

class KmallocHeap {
public:
    Std::Result<void> initialise();

    Std::Result<VirtualAddress> allocate(size_t size);

    Std::Result<VirtualAddress> allocate(size_t _size, int attempt);

    Std::Result<void> free(VirtualAddress address);

private:
    Std::Result<void> grow();

    KmallocSubHeap *m_sub_heaps{nullptr};

    Slab *m_sub_heap_allocator{nullptr};
};

}// namespace Kernel

extern Kernel::KmallocHeap g_malloc_heap;
