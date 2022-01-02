// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "MemoryManager.h"
#include "../Debugging.h"
#include <Try.h>

namespace Kernel {
void MemoryManager::init(const BootState &boot_state) {
    // Set up the basic bitmap frame allocator
    m_bitmap.init(Page, boot_state.physical_memory_size, 0, boot_state.kernel_address_space.frame_allocator.virtual_base);

    // Parse the memory map and block out any used memory regions
    for (auto &descriptor: boot_state.memory_map) {
        switch (descriptor.type) {
            // Consider these memory map types as 'used' and block them out of the bitmap
            case EFI::EFI_PAL_CODE:
            case EFI::EFI_LOADER_DATA:
            case EFI::EFI_BOOT_SERVICES_DATA:
            case EFI::EFI_RUNTIME_SERVICES_CODE:
            case EFI::EFI_RUNTIME_SERVICES_DATA:
            case EFI::EFI_ACPI_RECLAIM_MEMORY:
            case EFI::EFI_ACPI_MEMORY_NVS:
            case EFI::EFI_UNUSABLE_MEMORY:
            case EFI::EFI_PERSISTENT_MEMORY:
                for (int i = 0; i < descriptor.number_of_pages; i++) {
                    auto address = PhysicalAddress::from_base_and_page_offset(descriptor.physical_start, i);
                    m_bitmap.set_allocated(address);
                }
                break;
            // Everything else, we consider as free memory
            default:
                break;
        }
    }

    m_kernel_page_directory = reinterpret_cast<PageDirectoryEntry *>(boot_state.kernel_address_space.kernel_page_directory_virtual_address);

    // Unmap the identity mapped memory region
    auto *pml4 = reinterpret_cast<PML4Entry *>(boot_state.kernel_address_space.initial_pages.virtual_base);
    pml4[0].present = 0;
    pml4[0].writeable = 0;
    pml4[0].physical_address = 0;
    invalidate_entire_tlb();

    // Set the start of the kernel heap virtual address space
    auto heap_virtual_base = VirtualAddress(boot_state.kernel_address_space.frame_allocator.virtual_base + boot_state.kernel_address_space.frame_allocator.size).offset(Page);
    auto heap_size = VirtualAddress(0xffffffffffffffff).difference(heap_virtual_base);
    m_kernel_heap_address_space = VirtualAddressSpace(heap_virtual_base, heap_size);

    // TODO: Remap kernel code as R+X
    // TODO: Remap kernel data as R+W
    // TODO: Remap kernel readonly data as R
}

Result<PhysicalAddress> MemoryManager::allocate_physical_page() {
    return m_bitmap.allocate();
}

Result<void> MemoryManager::free_page(PhysicalAddress address) {
    return m_bitmap.free(address);
}

Result<VirtualAddress> MemoryManager::allocate_kernel_heap_page() {
    auto physical_address = TRY(allocate_physical_page());
    auto virtual_address = TRY(m_kernel_heap_address_space.take_page());
    TRY(map_kernel_page_directory(physical_address, virtual_address));
    return virtual_address;
}

Result<VirtualAddress> MemoryManager::allocate_kernel_heap_pages(size_t page_count) {
    auto virtual_address_range = TRY(m_kernel_heap_address_space.take_pages(page_count));
    for(size_t i = 0; i < page_count; i++) {
        auto physical_address = TRY(allocate_physical_page());
        auto page_virtual_address = virtual_address_range.offset_pages(i);
        TRY(map_kernel_page_directory(physical_address, page_virtual_address));
    }
    return virtual_address_range;
}

Result<void> MemoryManager::free_kernel_heap_page(VirtualAddress virtual_address) {
    auto physical_address = TRY(kernel_virtual_to_physical_address(virtual_address));
    free_page(physical_address);
    return unmap_kernel_page_directory(virtual_address);
}

Result<void> MemoryManager::map_kernel_page_directory(const PhysicalAddress &physical_address, const VirtualAddress &virtual_address) {
    auto pte = TRY(get_kernel_page_table_entry(virtual_address));
    pte->present = 1;
    pte->writeable = 1;
    pte->physical_address = physical_address.as_address() >> 12;
    invalidate_tlb(virtual_address);
    return {};
}

Result<void> MemoryManager::unmap_kernel_page_directory(const VirtualAddress &virtual_address) {
    auto pte = TRY(get_kernel_page_table_entry(virtual_address));
    pte->present = 0;
    pte->writeable = 0;
    pte->physical_address = 0;
    invalidate_tlb(virtual_address);
    return {};
}

Result<size_t> MemoryManager::virtual_address_to_page_directory_index(VirtualAddress address) {
    auto result = (address.as_address() & 0x3fe00000) >> 21;
    if (result > 511) {
        return Lib::Error::from_code(1);
    }
    return result;
}

Result<size_t> MemoryManager::virtual_address_to_page_table_index(VirtualAddress address) {
    auto result = (address.as_address() & 0x1ff000) >> 12;
    if (result > 511) {
        return Lib::Error::from_code(1);
    }
    return result;
}

Result<PhysicalAddress> MemoryManager::kernel_virtual_to_physical_address(const VirtualAddress &virtual_address) {
    return PhysicalAddress(0xdeadc0de);
}

Result<PageTableEntry *> MemoryManager::get_kernel_page_table_entry(const VirtualAddress &virtual_address) {
    auto kernel_page_directory_index = TRY(virtual_address_to_page_directory_index(virtual_address));
    auto pde = &m_kernel_page_directory[kernel_page_directory_index];
    if (!pde->present) {
        auto page_table_physical_address = TRY(allocate_physical_page());
        pde->present = 1;
        pde->writeable = 1;
        pde->physical_address = page_table_physical_address.as_address() >> 12;
    }
    auto pagetable_physical_address = PhysicalAddress(static_cast<uint64_t>(pde->physical_address) << 12);
    auto pagetable = pagetable_physical_address.as_ptr<PageTableEntry>();
    auto page_table_index = TRY(virtual_address_to_page_table_index(virtual_address));
    return &pagetable[page_table_index];
}

void MemoryManager::invalidate_tlb(const VirtualAddress &virtual_address) {
    asm volatile("invlpg (%0)"
                 : /* no output */
                 : "b"(virtual_address.as_address())
                 : "memory");
}

void MemoryManager::invalidate_entire_tlb() {
    asm volatile("push %%rax\n"
                 "mov %%cr3, %%rax\n"
                 "mov %%rax, %%cr3\n"
                 "pop %%rax"
                 : /* no output */
                 : /* no input */
                 : "memory");
}

Result<VirtualAddress> VirtualAddressSpace::take_page() {
    if (m_free_pages == 0)
        return Lib::Error::from_code(1);

    auto result = m_next_address;
    m_next_address = m_next_address.offset(Page);
    m_free_pages--;
    return result;
}

Result<VirtualAddress> VirtualAddressSpace::take_pages(size_t page_count) {
    auto virtual_address = TRY(take_page());
    for(int i = 1; i < page_count; i++) {
        TRY(take_page());
    }
    return virtual_address;
}

}// namespace Kernel
