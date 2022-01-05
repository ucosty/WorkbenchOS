// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "Bitmap.h"
#include "PhysicalAddress.h"
#include "VirtualAddress.h"
#include <BootState.h>
#include <EFI/MemoryMap.h>
#include <PageStructures.h>
#include <Result.h>

namespace Kernel {
class VirtualAddressSpace {
public:
    VirtualAddressSpace() = default;
    explicit VirtualAddressSpace(VirtualAddress base_address, size_t size) : m_base_address(base_address), m_next_address(base_address), m_size(size), m_free_pages(size / Page) {}
    [[nodiscard]] Result<VirtualAddress> take_page();
    [[nodiscard]] Result<VirtualAddress> take_pages(size_t);

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

    Result<VirtualAddress> allocate_kernel_heap_page();
    Result<VirtualAddress> allocate_kernel_heap_pages(size_t page_count);
    Result<void> free_kernel_heap_page(VirtualAddress virtual_address);

    Result<PhysicalAddress> allocate_physical_page();
    Result<void> free_page(PhysicalAddress address);
    Result<PhysicalAddress> kernel_virtual_to_physical_address(const VirtualAddress &);
    Result<PageTableEntry *> get_kernel_page_table_entry(const VirtualAddress &);

    static Result<size_t> virtual_address_to_page_directory_index(VirtualAddress address);
    static Result<size_t> virtual_address_to_page_table_index(VirtualAddress address);

    Result<void> map_kernel_page_directory(const PhysicalAddress &, const VirtualAddress &);
    Result<void> unmap_kernel_page_directory(const VirtualAddress &);

    bool is_allocated(PhysicalAddress address);

    static void invalidate_tlb(const VirtualAddress &);
    static void invalidate_entire_tlb();
    void unmap_identity_mapping();

private:
    MemoryManager() = default;
    Bitmap m_bitmap{};
    VirtualAddressSpace m_kernel_heap_address_space;
    PageDirectoryEntry *m_kernel_page_directory{nullptr};
    PML4Entry *m_initial_pml4{nullptr};
};
}// namespace Kernel
