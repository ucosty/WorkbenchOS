// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "SlabAllocator.h"
#include <Result.h>
#include <Types.h>
#include <VirtualAddress.h>
#include <VirtualAddressRange.h>

#define ALLOC_BLOCK_SIZE 8

namespace Kernel {
class AllocatedBlock {
public:
    AllocatedBlock(size_t data_size) : m_data_size(data_size) {}

private:
    friend class KmallocSubHeap;

    uint64_t m_canary{0xd1d1d1d1d1d1d1d1};
    size_t m_data_size;
};

class FreeBlock {
public:
    FreeBlock(size_t size) : m_block_size(size) {}

    [[nodiscard]] FreeBlock *next() const { return m_next; };

    [[nodiscard]] size_t allocatable_size() const { return m_block_size - sizeof(AllocatedBlock); }

    void coalesce_with_next_block();

    void coalesce_with_previous_block();

    [[nodiscard]] size_t block_size() const {
        return m_block_size;
    }

    [[nodiscard]] bool is_canary_valid() const {
        return m_canary == 0xfefefefefefefefe;
    }

    bool can_coalesce_next();

    bool can_coalesce_previous();

    void split_if_required(size_t allocation_request);

private:
    friend class KmallocSubHeap;

    uint64_t m_canary{0xfefefefefefefefe};
    uint64_t m_block_size;
    FreeBlock *m_previous{nullptr};
    FreeBlock *m_next{nullptr};
};


class KmallocSubHeap {
public:
    Result<void> initialise();

    Result<void> initialise(KmallocSubHeap *next);

    Result<VirtualAddress> allocate(size_t _size);

    Result<void> free(VirtualAddress address);

    void insert_into_free_list(FreeBlock *free_block_to_insert);

    void remove_from_free_list(FreeBlock *free_block);

    bool can_allocate(size_t size) { return m_available >= size; }

    KmallocSubHeap *next() { return m_next; }

    bool contains_allocation(VirtualAddress address);

private:
    uint8_t *m_storage;
    FreeBlock *m_free_list;
    KmallocSubHeap *m_previous{nullptr};
    KmallocSubHeap *m_next{nullptr};
    size_t m_capacity;
    size_t m_available;
};

class KmallocHeap {
public:
    Result<void> initialise();

    Result<VirtualAddress> allocate(size_t size);

    Result<VirtualAddress> allocate(size_t _size, int attempt);

    Result<void> free(VirtualAddress address);

private:
    Result<void> grow();

    KmallocSubHeap *m_subheaps;

    Slab *m_subheap_allocator{nullptr};
};

}// namespace Kernel

extern Kernel::KmallocHeap g_malloc_heap;
