// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Kmalloc.h"
#include "../Debugging.h"
#include "../Memory/MemoryManager.h"
#include "LibStd/Try.h"
#include "LibStd/CString.h"

using namespace Std;

Kernel::KmallocHeap g_malloc_heap{};

namespace Kernel {
void FreeBlock::coalesce_with_next_block() {
    if (!can_coalesce_next())
        return;

    m_block_size += m_next->block_size();

    m_next = m_next->m_next;
    if (m_next != nullptr) {
        m_next->m_previous = this;
    }
}

void FreeBlock::coalesce_with_previous_block() {
    if (!can_coalesce_previous())
        return;

    m_previous->coalesce_with_next_block();
}

bool FreeBlock::can_coalesce_next() {
    if (m_next == nullptr)
        return false;
    auto end_of_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<uint8_t *>(this) + block_size());
    return m_next == end_of_block;
}

bool FreeBlock::can_coalesce_previous() {
    if (m_previous == nullptr)
        return false;

    auto end_of_previous_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<uint8_t *>(m_previous) +
                                                               m_previous->block_size());
    return end_of_previous_block == this;
}

/**
 * Split this block into two parts, one with an allocatable_size == allocation_request,
 * and another with the remainder of the allocatable allocation_request.
 *
 * @param allocation_request required allocatable allocation_request of the left-hand block
 *
 * Before:
 *      +--------------------------------------------------------------------------+
 *      | Metadata | Allocatable Space                                             |
 *      +--------------------------------------------------------------------------+
 *
 * After:
 *      +--------------------------------------------------------------------------+
 *      | Metadata | (allocation_request) space | Metadata | remainder space       |
 *      +--------------------------------------------------------------------------+
 */
void FreeBlock::split_if_required(size_t allocation_request) {
    if (allocation_request == allocatable_size())
        return;

    auto right_hand_size = allocatable_size() - allocation_request;

    // Not enough space to warrant a split_if_required, the remainder can't hold the metadata
    if (right_hand_size < sizeof(FreeBlock))
        return;

    // Resize the block
    m_block_size = allocation_request + sizeof(FreeBlock);

    auto *left_side_storage = reinterpret_cast<uint8_t *>(this) + sizeof(AllocatedBlock);
    auto *rhs_pointer = reinterpret_cast<FreeBlock *>(left_side_storage + allocation_request);
    auto right_hand_side = new (rhs_pointer) FreeBlock(right_hand_size);

    right_hand_side->m_previous = this;
    right_hand_side->m_next = this->m_next;
    this->m_next = right_hand_side;
}

Result<void> KmallocSubHeap::initialise() {
    auto sub_heap_size_pages = 0x100000 / Page;
    auto &memory_manager = MemoryManager::get_instance();
    m_storage = TRY(memory_manager.allocate_kernel_heap_pages(sub_heap_size_pages)).as_ptr();
    m_capacity = 0x100000;
    m_free_list = new (m_storage) FreeBlock(m_capacity);
    m_available = m_capacity;
    return {};
}

Result<void> KmallocSubHeap::initialise(KmallocSubHeap *next) {
    m_next = next;
    return initialise();
}

void KmallocSubHeap::remove_from_free_list(FreeBlock *free_block) {
    auto next_free_block = free_block->m_next;
    if (next_free_block != nullptr) {
        next_free_block->m_previous = free_block->m_previous;
    }

    auto previous_free_block = free_block->m_previous;
    if (previous_free_block != nullptr) {
        previous_free_block->m_next = free_block->m_next;
    }

    if (free_block == m_free_list) {
        m_free_list = free_block->next();
    }
}

void KmallocSubHeap::insert_into_free_list(FreeBlock *free_block_to_insert) {
    if (m_free_list == nullptr) {
        m_free_list = free_block_to_insert;
        return;
    }

    if (free_block_to_insert < m_free_list) {
        free_block_to_insert->m_next = m_free_list;
        m_free_list->m_previous = free_block_to_insert;
        m_free_list = free_block_to_insert;
        return;
    }

    auto free_block = m_free_list;
    while (free_block != nullptr) {
        if (free_block_to_insert > free_block) {
            if (free_block->m_next == nullptr || free_block_to_insert < free_block->m_next) {
                free_block_to_insert->m_next = free_block->m_next;
                free_block_to_insert->m_previous = free_block;
                free_block->m_next = free_block_to_insert;
            }
        }
        free_block = free_block->next();
    }
}

Result<void> KmallocSubHeap::free(VirtualAddress address) {
    if (address.as_ptr() == nullptr)
        return {};

    auto allocated_block = reinterpret_cast<AllocatedBlock *>(address.as_ptr() - sizeof(AllocatedBlock));

    if (allocated_block->m_canary != 0xd1d1d1d1d1d1d1d1) {
        return Error::from_code(1);
    }

    auto block_size = allocated_block->m_data_size + sizeof(AllocatedBlock);
    auto free_block = new (allocated_block) FreeBlock(block_size);

    insert_into_free_list(free_block);
    free_block->coalesce_with_next_block();
    free_block->coalesce_with_previous_block();
    return {};
}

Result<VirtualAddress> KmallocSubHeap::allocate(size_t _size) {
    size_t size = (_size < sizeof(FreeBlock)) ? sizeof(FreeBlock) : _size;
    auto free_block = m_free_list;
    while (free_block != nullptr) {
        if (!free_block->is_canary_valid()) {
            return Error::from_code(1);
        }

        if (size <= free_block->allocatable_size()) {
            free_block->split_if_required(size);
            remove_from_free_list(free_block);
            auto allocated_block = new (free_block) AllocatedBlock(size);
            return VirtualAddress(allocated_block + 1);
        }

        free_block = free_block->next();
    }

    return Error::from_code(1);
}

bool KmallocSubHeap::contains_allocation(VirtualAddress address) {
    auto start = reinterpret_cast<void *>(m_storage);
    auto end = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(m_storage) + m_capacity);
    return address.as_ptr() >= start && address.as_ptr() < end;
}

Result<void> KmallocHeap::initialise() {
    auto &slab_allocator = SlabAllocator::get_instance();
    m_subheap_allocator = TRY(slab_allocator.get_or_create_slab(sizeof(KmallocSubHeap))).as_ptr();
    m_subheaps = TRY(m_subheap_allocator->allocate<KmallocSubHeap>());
    m_subheaps->initialise();
    return {};
}

Result<VirtualAddress> KmallocHeap::allocate(size_t size) {
    if(size == 0) {
        return Error::with_message("Cannot allocate zero byte buffer"_sv);
    }
    return allocate(size, 0);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
Result<VirtualAddress> KmallocHeap::allocate(size_t size, int attempt) {
    if (attempt == 3)
        return Error::from_code(1);

    auto sub_heap = m_subheaps;
    while (sub_heap != nullptr) {
        if (sub_heap->can_allocate(size)) {
            return sub_heap->allocate(size);
        }
        sub_heap = sub_heap->next();
    }

    TRY(grow());
    return allocate(size, attempt + 1);
}
#pragma clang diagnostic pop

Result<void> KmallocHeap::free(VirtualAddress address) {
    auto sub_heap = m_subheaps;
    while (sub_heap != nullptr) {
        if (sub_heap->contains_allocation(address)) {
            return sub_heap->free(address);
        }
        sub_heap = sub_heap->next();
    }
    return Error::from_code(1);
}

Result<void> KmallocHeap::grow() {
    auto next_subheap = TRY(m_subheap_allocator->allocate<KmallocSubHeap>());
    next_subheap->initialise(m_subheaps);
    m_subheaps = next_subheap;
    return {};
}
}// namespace Kernel

void *operator new(size_t size) {
    auto allocation = g_malloc_heap.allocate(size);
    if(allocation.is_error()) {
        return nullptr;
    }
    return reinterpret_cast<void *>(allocation.get().as_ptr());
}

void *operator new[](size_t size) {
    auto allocation = g_malloc_heap.allocate(size);
    if(allocation.is_error()) {
        return nullptr;
    }
    return reinterpret_cast<void *>(allocation.get().as_ptr());
}

void operator delete(void *ptr) noexcept {
    g_malloc_heap.free(VirtualAddress(ptr));
}

void operator delete[](void *ptr) noexcept {
    g_malloc_heap.free(VirtualAddress(ptr));
}