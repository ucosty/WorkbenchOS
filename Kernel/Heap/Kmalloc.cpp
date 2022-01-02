// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Kmalloc.h"
#include "../Debugging.h"
#include "../Memory/MemoryManager.h"
#include <ConsoleIO.h>
#include <Try.h>
#include <cstring.h>

Kernel::KmallocHeap g_malloc_heap{};

namespace Kernel {
Result<void> KmallocHeap::initialise() {
    auto &slab_allocator = SlabAllocator::get_instance();
    m_subheap_allocator = TRY(slab_allocator.get_or_create_slab(sizeof(KmallocSubheap))).as_ptr();
    m_subheap_head = TRY(m_subheap_allocator->allocate<KmallocSubheap>());
    m_subheap_end = m_subheap_head;
    m_subheap_head->initialise();
    return {};
}

Result<VirtualAddress> KmallocHeap::allocate(size_t size, int attempt = 0) {
    if (attempt == 3)
        return Lib::Error::from_code(1);

    auto *subheap = m_subheap_head;
    while (subheap != nullptr) {
        auto result = subheap->allocate(size);
        if (!result.is_error()) {
            return result;
        }
        subheap = subheap->next();
    }
    TRY(new_subheap());
    return allocate(size, attempt + 1);
}

Result<void> KmallocHeap::free(const VirtualAddress &address) {
    auto *subheap = m_subheap_head;
    while (subheap != nullptr) {
        if (subheap->in_subheap(address)) {
            return subheap->free(address);
        }
    }
    printf("PANIC: Could not find block to free in heap %X\n", address.as_address());
    return Lib::Error::from_code(1);
}

Result<void> KmallocHeap::new_subheap() {
    auto next_subheap = TRY(m_subheap_allocator->allocate<KmallocSubheap>());
    m_subheap_end->set_next_subheap(next_subheap);
    next_subheap->initialise();
    return {};
}

Result<void> KmallocSubheap::initialise() {
    auto &memory_manager = MemoryManager::get_instance();
    m_storage = TRY(memory_manager.allocate_kernel_heap_pages(subheap_size_pages)).as_ptr();
    m_free_list = reinterpret_cast<FreeBlock *>(m_storage);
    m_free_list->canary = canary_free;
    m_free_list->size = subheap_size - sizeof(FreeBlock);
    m_free_list->previous = nullptr;
    m_free_list->next = nullptr;
    m_address_range = VirtualAddressRange(VirtualAddress(reinterpret_cast<uint64_t>(m_storage)), subheap_size_pages);
    return {};
}

/*  Used Block               Free Block
 * +------------------------+---------------------------------+
 * | canary | size | <data> | canary | size | next_free       |
 * +------------------------+---------------------------------+
 */
Result<VirtualAddress> KmallocSubheap::allocate(size_t size) {
    if (m_free_list == nullptr)
        return Lib::Error::from_code(1);

    auto remainder = size % ALLOC_BLOCK_SIZE;
    if (remainder > 0) {
        size += ALLOC_BLOCK_SIZE - remainder;
    }

    FreeBlock *best_fit = m_free_list;
    FreeBlock *current_block = m_free_list;

    // Walk the free list and find a block big enough to allocate
    while (current_block != nullptr) {
        if (current_block->canary != canary_free) {
            printf("PANIC: invalid canary on free list block at %X, found %X\n", current_block, current_block->canary);
            return Lib::Error::from_code(1);
        }

        if (current_block->size >= size && current_block->size < best_fit->size) {
            best_fit = current_block;
        }
        current_block = current_block->next;
    }

    // Split the found free block
    best_fit->split(size);

    // Convert the free block into an allocated block
    best_fit->remove_from_free_list();
    if (best_fit == m_free_list) {
        m_free_list = best_fit->next;
    }

    auto allocated_block = reinterpret_cast<AllocationMetadata *>(best_fit);
    return allocated_block->initialise(size);
}

Result<void> KmallocSubheap::free(const VirtualAddress &address) {
    auto allocated_block = reinterpret_cast<AllocationMetadata *>(address.as_address() - sizeof(AllocationMetadata));
    VERIFY(allocated_block->canary == canary_allocated);
    auto free_block = reinterpret_cast<FreeBlock *>(allocated_block);
    auto block_size = allocated_block->size;

    // If there is no free list, create one from this block
    if (m_free_list == nullptr) {
        free_block->add_to_free_list(nullptr, block_size);
        m_free_list = free_block;
        return {};
    }

    // An existing free list exists
    VERIFY(m_free_list->previous == nullptr);
    free_block->next = m_free_list;
    m_free_list->previous = free_block;
    free_block->add_to_free_list(m_free_list, block_size);
    m_free_list = free_block;
    return {};
}

void KmallocSubheap::set_next_subheap(KmallocSubheap *next) {
    if (m_next == nullptr)
        m_next = next;
}

bool KmallocSubheap::in_subheap(const VirtualAddress &address) {
    return m_address_range.in_range(address);
}

void FreeBlock::split(size_t lhs_size) {
    if (lhs_size == size)
        return;

    auto rhs_size = size - lhs_size;
    size = lhs_size;

    auto rhs_address = reinterpret_cast<uint64_t>(this) + sizeof(FreeBlock) + size;
    auto rhs = reinterpret_cast<FreeBlock *>(rhs_address);
    rhs->canary = canary_free;
    rhs->size = rhs_size - sizeof(FreeBlock);

    rhs->next = next;
    rhs->previous = this;
    next = rhs;
}

void FreeBlock::add_to_free_list(FreeBlock *_next, size_t _size) {
    previous = nullptr;
    next = _next;
    size = _size;
    canary = canary_free;
}

void FreeBlock::remove_from_free_list() const {
    if (previous != nullptr)
        previous->next = next;

    if (next != nullptr)
        next->previous = previous;
}

VirtualAddress AllocationMetadata::initialise(size_t _size) {
    uint64_t address = reinterpret_cast<uint64_t>(this) + sizeof(AllocationMetadata);
    canary = canary_allocated;
    size = _size;
    memset(reinterpret_cast<char *>(address), 0, size);
    return VirtualAddress(address);
}
}// namespace Kernel

void* operator new(size_t size) {
    auto allocation = g_malloc_heap.allocate(size);
    if(allocation.is_error()) {
        return nullptr;
    }
    return reinterpret_cast<void *>(allocation.get().as_ptr());
}

void operator delete(void *ptr) {
    g_malloc_heap.free(VirtualAddress(ptr));
}
