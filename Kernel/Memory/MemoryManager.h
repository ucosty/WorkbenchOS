// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "Bitmap.h"
#include "PhysicalAddress.h"
#include "VirtualAddress.h"
#include <BootState.h>
#include <PageStructures.h>
#include "LibStd/Result.h"

namespace Kernel {
    struct MemoryManagerError {
        enum class Kind { AllocatorError, GenericError };
        Kind kind;

        MemoryManagerError() = delete;
        explicit MemoryManagerError(const Kind kind) {
            this->kind = kind;
        }
    };

class VirtualAddressSpace {
public:
    VirtualAddressSpace() = default;
    explicit VirtualAddressSpace(VirtualAddress base_address, size_t size) : m_base_address(base_address), m_next_address(base_address), m_size(size), m_free_pages(size / Page) {}
    [[nodiscard]] Std::Result<VirtualAddress, MemoryManagerError> take_page();
    [[nodiscard]] Std::Result<VirtualAddress, MemoryManagerError> take_pages(size_t);

private:
    VirtualAddress m_base_address;
    VirtualAddress m_next_address;
    size_t m_size{0};
    size_t m_free_pages{0};
};

class MemoryManager {
public:
    static MemoryManager &get_instance() {
        static MemoryManager instance;
        return instance;
    }
    MemoryManager(MemoryManager const &) = delete;
    void operator=(MemoryManager const &) = delete;

    void init(const BootState &boot_state);

    Std::Result<VirtualAddress, MemoryManagerError> allocate_kernel_heap_page();
    Std::Result<VirtualAddress, MemoryManagerError> allocate_kernel_heap_pages(size_t page_count);
    Std::Result<void, MemoryManagerError> free_kernel_heap_page(VirtualAddress virtual_address);

    Std::Result<PhysicalAddress, MemoryManagerError> allocate_physical_page();
    Std::Result<PhysicalAddress, MemoryManagerError> allocate_physical_pages(size_t page_count);
    Std::Result<void, MemoryManagerError> free_page(PhysicalAddress address);
    [[nodiscard]] Std::Result<PhysicalAddress, MemoryManagerError> kernel_virtual_to_physical_address(const VirtualAddress &) const;
    Std::Result<PhysicalAddress, MemoryManagerError> kernel_virtual_to_physical_address(u64 address);
    Std::Result<PageTableEntry *, MemoryManagerError> get_kernel_page_table_entry(const VirtualAddress &);

    static Std::Result<size_t, MemoryManagerError> virtual_address_to_page_directory_index(VirtualAddress address);
    static Std::Result<size_t, MemoryManagerError> virtual_address_to_page_table_index(VirtualAddress address);

    Std::Result<void, MemoryManagerError> map_kernel_page_directory(const PhysicalAddress &, const VirtualAddress &);
    Std::Result<void, MemoryManagerError> unmap_kernel_page_directory(const VirtualAddress &);

    Std::Result<PhysicalAddress, MemoryManagerError> create_user_mode_directory();
    Std::Result<void, MemoryManagerError> map_user_page(PhysicalAddress pdpt_physical_address, const PhysicalAddress &, const VirtualAddress &, bool write, bool execute);
    Std::Result<void, MemoryManagerError> set_user_directory(PhysicalAddress address);


    Std::Result<VirtualAddress, MemoryManagerError> allocate_user_pages(PhysicalAddress pdpt, u64 virtual_base, size_t page_count, bool write, bool execute);


    Std::Result<void, MemoryManagerError> set_page_flags(VirtualAddress address, size_t page_count, bool write, bool execute);

    static void invalidate_tlb(const VirtualAddress &);
    static void invalidate_entire_tlb();
    void unmap_identity_mapping() const;

private:
    MemoryManager() = default;
    Bitmap m_bitmap{};
    VirtualAddressSpace m_kernel_heap_address_space;
    PageDirectoryEntry *m_kernel_page_directory{nullptr};
    PML4Entry *m_pml4{nullptr};
};
}// namespace Kernel

namespace Std {
    template <>
    struct FromError<Kernel::MemoryManagerError, Kernel::BitmapError> {
        static constexpr Kernel::MemoryManagerError from(Kernel::BitmapError e) {
            return Kernel::MemoryManagerError{Kernel::MemoryManagerError::Kind::AllocatorError};
        }
    };

    template <>
    struct FromError<Kernel::MemoryManagerError, Error> {
        static constexpr Kernel::MemoryManagerError from(Error e) {
            return Kernel::MemoryManagerError{Kernel::MemoryManagerError::Kind::GenericError};
        }
    };

    template <>
    struct FromError<Error, Kernel::MemoryManagerError> {
        static constexpr Error from(Kernel::MemoryManagerError e) {
            return Error::from_code(1);
        }
    };

}
