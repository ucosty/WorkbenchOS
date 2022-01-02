// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "VirtualAddress.h"
#include "Try.h"
#include <NonNullPtr.h>
#include <Result.h>
#include <Types.h>

namespace Kernel {

struct FreeObject {
    FreeObject *next;
};

static_assert(sizeof(FreeObject) == 8);

// Stored at the end of the slab page
// | < 0                                               0x1000 > |
// +------------------------------------------------------------+
// | Item | Item | Item | Item | Item | Item | Slack | Metadata |
// +------------------------------------------------------------+
class SlabPage {
public:
    Result<void> initialise(size_t size);
    [[nodiscard]] SlabPage *next() const { return m_next_page; }
    [[nodiscard]] SlabPage *previous() const { return m_previous_page; }
    [[nodiscard]] bool has_next() const { return m_next_page != nullptr; }
    [[nodiscard]] bool has_free_objects() const { return m_free_objects > 0; }
    [[nodiscard]] size_t total_objects() const { return m_total_objects; }
    [[nodiscard]] size_t free_objects() const { return m_free_objects; }
    [[nodiscard]] VirtualAddress get_object_address(size_t index) const {
        auto page_address = reinterpret_cast<size_t>(this);
        return VirtualAddress(page_address + sizeof(SlabPage) + (m_object_size * index));
    }
    [[nodiscard]] Result<uint8_t *> allocate();
    Result<void> free(VirtualAddress address);
    [[nodiscard]] bool contains_object_at_address(VirtualAddress address) const {
        auto base_address = reinterpret_cast<uint64_t>(this);
        return (address.as_address() > base_address) &&
            (address.as_address() < base_address + 0x1000);
    }

private:
    size_t m_object_size;
    size_t m_free_objects;
    size_t m_total_objects;
    SlabPage *m_next_page{nullptr};
    SlabPage *m_previous_page{nullptr};
    FreeObject *m_free_list{nullptr};
};

static_assert(sizeof(SlabPage) == 48);

class Slab {
public:
    Result<void> initialise(size_t size);
    [[nodiscard]] bool is_slab_for(size_t size) const { return m_initialised && m_object_size == size; }
    [[nodiscard]] bool is_initialised() const { return m_initialised; }
    [[nodiscard]] Result<NonNullPtr<uint8_t>> allocate();
    template<typename T>
    Result<T *> allocate() {
        auto result = TRY(allocate());
        return reinterpret_cast<T *>(result.as_address());
    }
    [[nodiscard]] Result<void> free(VirtualAddress address);

private:
    SlabPage *m_head{nullptr};
    bool m_initialised{false};
    size_t m_object_size{0};
    char *m_name{nullptr};
};

class SlabAllocator {
public:
    static SlabAllocator &get_instance() {
        static SlabAllocator instance;
        return instance;
    }
    SlabAllocator(SlabAllocator const &) = delete;
    void operator=(SlabAllocator const &) = delete;

    Result<NonNullPtr<Slab>> get_or_create_slab(size_t size);

private:
    SlabAllocator() = default;
    Result<NonNullPtr<Slab>> find_slab(size_t size);
    Result<NonNullPtr<Slab>> create_slab(size_t size);
    Slab m_slabs[32];
};
}// namespace Kernel
