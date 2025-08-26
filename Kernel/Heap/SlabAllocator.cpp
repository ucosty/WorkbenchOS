// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "SlabAllocator.h"
#include "../Debugging.h"
#include "../Memory/MemoryManager.h"
#include "LibStd/Try.h"

using namespace Std;

namespace Kernel {
constexpr size_t MAX_OBJECT_SIZE = Page - sizeof(SlabPage);
Result<NonNullPtr<Slab>> SlabAllocator::get_or_create_slab(const size_t size) {
    VERIFY(size < MAX_OBJECT_SIZE);
    auto slab = find_slab(size);
    if (slab.is_error()) {
        return create_slab(size);
    }
    return slab;
}

Result<NonNullPtr<Slab>> SlabAllocator::find_slab(const size_t size) {
    for (auto &slab: m_slabs) {
        if (slab.is_slab_for(size)) {
            return NonNullPtr<Slab>::from(&slab);
        }
    }
    return Error::from_code(1);
}

Result<NonNullPtr<Slab>> SlabAllocator::create_slab(const size_t size) {
    for (auto &slab: m_slabs) {
        if (!slab.is_initialised()) {
            TRY(slab.initialise(size));
            return NonNullPtr<Slab>::from(&slab);
        }
    }
    return Error::from_code(1);
}

Result<void> Slab::initialise(const size_t size) {
    auto &memory_manager = Kernel::MemoryManager::get_instance();
    m_initialised = true;
    m_object_size = size;
    const auto page_address = TRY(memory_manager.allocate_kernel_heap_page());
    m_head = reinterpret_cast<SlabPage *>(page_address.as_ptr());
    m_head->initialise(size);
    return {};
}

Result<NonNullPtr<u8>> Slab::allocate() const {
    VERIFY(m_head != nullptr);
    for (auto page = m_head; page != nullptr; page = page->next()) {
        if (page->has_free_objects()) {
            const auto object = TRY(page->allocate());
            return NonNullPtr<u8>::from(object);
        }
    }
    return Error::from_code(1);
}

Result<void> Slab::free(const VirtualAddress address) const {
    for (auto page = m_head; page != nullptr; page = page->next()) {
        if (page->contains_object_at_address(address)) {
            TRY(page->free(address));
            return {};
        }
    }
    return Error::from_code(1);
}

Result<void> SlabPage::initialise(const size_t size) {
    m_object_size = size;
    m_total_objects = (Page - sizeof(SlabPage)) / size;
    m_free_objects = m_total_objects;
    for (int i = 0; i < m_total_objects; i++) {
        auto object_address = get_object_address(i);
        const auto next_object_address = i < m_total_objects ? get_object_address(i + 1).as_address() : 0;
        const auto free_object = reinterpret_cast<FreeObject *>(object_address.as_address());
        free_object->next = reinterpret_cast<FreeObject *>(next_object_address);
    }
    m_free_list = reinterpret_cast<FreeObject *>(get_object_address(0).as_address());
    return {};
}

Result<u8 *> SlabPage::allocate() {
    if (m_free_list == nullptr) {
        return Error::from_code(1);
    }
    const auto free_object = m_free_list;
    m_free_list = free_object->next;
    m_free_objects--;
    return reinterpret_cast<u8 *>(free_object);
}

Result<void> SlabPage::free(const VirtualAddress address) {
    const auto free_object = reinterpret_cast<FreeObject *>(address.as_address());
    if (m_free_list == nullptr) {
        m_free_list = free_object;
        m_free_list->next = nullptr;
    } else {
        free_object->next = m_free_list;
        m_free_list = free_object;
    }
    m_free_objects++;
    return {};
}
}// namespace Kernel
