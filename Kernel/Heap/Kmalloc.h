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
constexpr uint64_t canary_allocated = 0xf1f1f1f1f1f1f1f1;
constexpr uint64_t canary_free = 0xfefefefefefefefe;

struct FreeBlock {
    uint64_t canary;
    size_t size;
    FreeBlock *previous;
    FreeBlock *next;

    void split(size_t lhs_size);
    void remove_from_free_list() const;
    void add_to_free_list(FreeBlock *_next, size_t _size);
};

struct AllocationMetadata {
    uint64_t canary;
    size_t size;

    VirtualAddress initialise(size_t _size);
};

static_assert(sizeof(AllocationMetadata) == 16);
static_assert(sizeof(FreeBlock) == 32);

class KmallocSubheap {
public:
    Result<void> initialise();
    Result<VirtualAddress> allocate(size_t size);
    Result<void> free(const VirtualAddress &);
    [[nodiscard]] KmallocSubheap *next() const { return m_next; }
    void set_next_subheap(KmallocSubheap *);
    bool in_subheap(const VirtualAddress &);

private:
    static constexpr size_t subheap_size = 10 * MiB;
    static constexpr size_t subheap_size_pages = subheap_size / Page;

    VirtualAddressRange m_address_range{};
    uint8_t *m_storage{nullptr};
    KmallocSubheap *m_next{nullptr};
    FreeBlock *m_free_list{nullptr};
};

class KmallocHeap {
public:
    Result<void> initialise();
    Result<VirtualAddress> allocate(size_t size, int);
    Result<void> free(const VirtualAddress &);
    Result<void> new_subheap();

private:
    KmallocSubheap *m_subheap_head{nullptr};
    KmallocSubheap *m_subheap_end{nullptr};
    Slab *m_subheap_allocator{nullptr};
};
}// namespace Kernel

extern Kernel::KmallocHeap g_malloc_heap;
