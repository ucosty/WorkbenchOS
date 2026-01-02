// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "MemoryManager.h"
#include "../Debugging.h"
#include "LibStd/Try.h"


using namespace Std;

namespace Kernel {
    void MemoryManager::init(const BootState &boot_state) {
        // Set up the basic bitmap frame allocator
        m_bitmap.init(Page, boot_state.physical_memory_size, 0,
                      boot_state.kernel_address_space.frame_allocator.virtual_base);

        // Parse the memory map and block out any used memory regions
        for (const auto &descriptor: boot_state.memory_map) {
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
                        const auto address = PhysicalAddress::from_base_and_page_offset(descriptor.physical_start, i);
                        m_bitmap.set_allocated(address);
                    }
                    break;
                // Everything else, we consider as free memory
                default:
                    break;
            }
        }

        // Reserve all memory below 1 MiB for the APs
        for (int i = 0; i < 256; i++) {
            const auto address = PhysicalAddress::from_base_and_page_offset(0, i);
            m_bitmap.set_allocated(address);
        }

        m_pml4 = reinterpret_cast<PML4Entry *>(boot_state.kernel_address_space.initial_pages.virtual_base);
        m_kernel_page_directory = reinterpret_cast<PageDirectoryEntry *>(boot_state.kernel_address_space.
            kernel_page_directory_virtual_address);

        // Set the start of the kernel heap virtual address space
        const auto heap_virtual_base = VirtualAddress(
            boot_state.kernel_address_space.frame_allocator.virtual_base + boot_state.kernel_address_space.
            frame_allocator.size).offset(Page);
        const auto heap_size = VirtualAddress(0xffffffffffffffff).difference(heap_virtual_base);
        m_kernel_heap_address_space = VirtualAddressSpace(heap_virtual_base, heap_size);

        println("Kernel heap: {:#x} -> {:#x}", heap_virtual_base.as_address(),
                heap_virtual_base.offset(heap_size).as_address());


        println("Region: name = kernel, start = {:#x}, end = {:#x}",
                boot_state.kernel_address_space.kernel.virtual_base,
                boot_state.kernel_address_space.kernel.virtual_base + boot_state.kernel_address_space.kernel.size);
        println("Region: name = boot_state, start = {:#x}, end = {:#x}",
                boot_state.kernel_address_space.boot_state.virtual_base,
                boot_state.kernel_address_space.boot_state.virtual_base + boot_state.kernel_address_space.boot_state.
                size);
        println("Region: name = initial_pages, start = {:#x}, end = {:#x}",
                boot_state.kernel_address_space.initial_pages.virtual_base,
                boot_state.kernel_address_space.initial_pages.virtual_base + boot_state.kernel_address_space.
                initial_pages.size);
        println("Region: name = memory_map, start = {:#x}, end = {:#x}",
                boot_state.kernel_address_space.memory_map.virtual_base,
                boot_state.kernel_address_space.memory_map.virtual_base + boot_state.kernel_address_space.memory_map.
                size);
        println("Region: name = framebuffer, start = {:#x}, end = {:#x}",
                boot_state.kernel_address_space.framebuffer.virtual_base,
                boot_state.kernel_address_space.framebuffer.virtual_base + boot_state.kernel_address_space.framebuffer.
                size);
        println("Region: name = stack, start = {:#x}, end = {:#x}", boot_state.kernel_address_space.stack.virtual_base,
                boot_state.kernel_address_space.stack.virtual_base + boot_state.kernel_address_space.stack.size);
        println("Region: name = frame_allocator, start = {:#x}, end = {:#x}",
                boot_state.kernel_address_space.frame_allocator.virtual_base,
                boot_state.kernel_address_space.frame_allocator.virtual_base + boot_state.kernel_address_space.
                frame_allocator.size);

        for (auto i = 0; i < boot_state.kernel_address_space.program_header_count; i++) {
            const auto address = VirtualAddress(boot_state.kernel_address_space.program_header_flags[i].address);
            const auto page_count = boot_state.kernel_address_space.program_header_flags[i].size / 0x1000;
            const auto flags = boot_state.kernel_address_space.program_header_flags[i].flags;

            println("Mapping range {:#x} -> {:#x} to read = 1, write = {}, execute = {}",
                    boot_state.kernel_address_space.program_header_flags[i].address,
                    boot_state.kernel_address_space.program_header_flags[i].address + (page_count * 0x1000),
                    (flags & 0x2) >> 1, flags & 0x1);

            set_page_flags(address, page_count, (flags & 0x2) >> 1, flags & 0x1);
        }

        const auto stack_page_count = boot_state.kernel_address_space.stack.size / 0x1000;
        set_page_flags(VirtualAddress(boot_state.kernel_address_space.stack.virtual_base), stack_page_count, true,
                       false);
        println("Configured stack: size = {} pages, range = {:#x} -> {:#x}", stack_page_count,
                boot_state.kernel_address_space.stack.virtual_base,
                boot_state.kernel_address_space.stack.virtual_base + boot_state.kernel_address_space.stack.size);
    }

    Result<PhysicalAddress, MemoryManagerError> MemoryManager::allocate_physical_page() {
        const auto result = TRY_INTO(MemoryManagerError, m_bitmap.allocate());
        return result;
    }

    Result<PhysicalAddress, MemoryManagerError> MemoryManager::allocate_physical_pages(const size_t page_count) {
        const auto result = m_bitmap.allocate_contiguous(page_count);
        if (result.is_error()) {
            return MemoryManagerError(MemoryManagerError::Kind::GenericError);
        }
        return result.get();
    }

    Result<void, MemoryManagerError> MemoryManager::free_page(const PhysicalAddress address) {
        TRY_INTO(MemoryManagerError, m_bitmap.free(address));
        return {};
    }

    Result<VirtualAddress, MemoryManagerError> MemoryManager::allocate_kernel_heap_page() {
        const auto physical_address = TRY(allocate_physical_page());
        const auto virtual_address = TRY_INTO(MemoryManagerError, m_kernel_heap_address_space.take_page());
        TRY_INTO(MemoryManagerError, map_kernel_page_directory(physical_address, virtual_address));
        return virtual_address;
    }

    Result<VirtualAddress, MemoryManagerError> MemoryManager::allocate_kernel_heap_pages(size_t page_count) {
        const auto virtual_address_range = TRY_INTO(MemoryManagerError,
                                                    m_kernel_heap_address_space.take_pages(page_count));
        for (size_t i = 0; i < page_count; i++) {
            auto physical_address = TRY(allocate_physical_page());
            auto page_virtual_address = virtual_address_range.offset_pages(i);
            TRY(map_kernel_page_directory(physical_address, page_virtual_address));
        }
        return virtual_address_range;
    }

    Result<void, MemoryManagerError> MemoryManager::free_kernel_heap_page(VirtualAddress virtual_address) {
        const auto physical_address = TRY(kernel_virtual_to_physical_address(virtual_address));
        free_page(physical_address);
        return unmap_kernel_page_directory(virtual_address);
    }

    Result<void, MemoryManagerError> MemoryManager::map_kernel_page_directory(const PhysicalAddress &physical_address,
                                                                              const VirtualAddress &virtual_address) {
        const auto pte = TRY(get_kernel_page_table_entry(virtual_address));
        pte->present = 1;
        pte->writeable = 1;
        pte->physical_address = physical_address.as_address() >> 12;
        invalidate_tlb(virtual_address);
        return {};
    }

    Result<void, MemoryManagerError> MemoryManager::unmap_kernel_page_directory(const VirtualAddress &virtual_address) {
        const auto pte = TRY(get_kernel_page_table_entry(virtual_address));
        pte->present = 0;
        pte->writeable = 0;
        pte->physical_address = 0;
        invalidate_tlb(virtual_address);
        return {};
    }

    Result<size_t, MemoryManagerError> MemoryManager::virtual_address_to_page_table_index(const VirtualAddress address) {
        const auto result = (address.as_address() & 0x1ff000ull) >> 12;
        if (result > 511) {
            return MemoryManagerError(MemoryManagerError::Kind::GenericError);
        }
        return result;
    }

    Result<size_t, MemoryManagerError> MemoryManager::virtual_address_to_page_directory_index(
        const VirtualAddress address) {
        const auto result = (address.as_address() & 0x3fe00000ull) >> 21;
        if (result > 511) {
            // FIXME: make this error better
            return MemoryManagerError(MemoryManagerError::Kind::GenericError);
        }
        return result;
    }

    Result<PhysicalAddress, MemoryManagerError> MemoryManager::kernel_virtual_to_physical_address(
        const VirtualAddress &virtual_address) const {
        const auto page_directory_index = TRY(virtual_address_to_page_directory_index(virtual_address));
        const auto pde = &m_kernel_page_directory[page_directory_index];
        VERIFY(pde->present);

        // Get the page table from the page directory entry
        const auto pagetable_physical_address = PhysicalAddress(pde->physical_address << 12);
        const auto pagetable = pagetable_physical_address.as_ptr<PageTableEntry>();
        VERIFY(pagetable->present);

        // Get the page table index for the virtual address
        const auto page_table_index = TRY(virtual_address_to_page_table_index(virtual_address));
        const auto page = &pagetable[page_table_index];
        VERIFY(page->present);

        const auto address = page->physical_address << 12;

        // Get the lower 12 bits of the address from the virtual address
        const auto lower_bits = virtual_address.as_address() & 0xfff;
        return PhysicalAddress(address + lower_bits);
    }

    Result<PhysicalAddress, MemoryManagerError> MemoryManager::kernel_virtual_to_physical_address(const u64 address) {
        const VirtualAddress virtual_address(address);
        return kernel_virtual_to_physical_address(virtual_address);
    }


    Result<PageTableEntry *, MemoryManagerError> MemoryManager::get_kernel_page_table_entry(
        const VirtualAddress &virtual_address) {
        const auto kernel_page_directory_index = TRY(virtual_address_to_page_directory_index(virtual_address));
        const auto pde = &m_kernel_page_directory[kernel_page_directory_index];
        if (!pde->present) {
            const auto page_table_physical_address = TRY(allocate_physical_page());
            pde->present = 1;
            pde->writeable = 1;
            pde->physical_address = page_table_physical_address.as_address() >> 12;
        }
        auto pagetable_physical_address = PhysicalAddress(static_cast<u64>(pde->physical_address) << 12);
        auto pagetable = pagetable_physical_address.as_ptr<PageTableEntry>();
        auto page_table_index = TRY(virtual_address_to_page_table_index(virtual_address));
        return &pagetable[page_table_index];
    }

    Result<PhysicalAddress, MemoryManagerError> MemoryManager::create_user_mode_directory() {
        // Allocate page for the PDPT
        auto pdpt_physical_address = TRY(allocate_physical_page());
        auto pdpt = pdpt_physical_address.as_ptr<PageDirectoryPointerTableEntry>();

        // Allocate page for the page directory
        const auto page_directory_physical_address = TRY(allocate_physical_page());

        // Configure the PDPT entry
        pdpt[0].present = 1;
        pdpt[0].physical_address = page_directory_physical_address.as_address() >> 12;
        pdpt[0].writeable = 1;
        pdpt[0].user_access = 1;

        return pdpt_physical_address;
    }

    Result<void, MemoryManagerError> MemoryManager::map_user_page(PhysicalAddress pdpt_physical_address,
                                                                  const PhysicalAddress &physical_address,
                                                                  const VirtualAddress &virtual_address,
                                                                  bool write, bool execute) {
        auto pdpt = pdpt_physical_address.as_ptr<PageDirectoryPointerTableEntry>();
        auto pd = pdpt[0].get_physical_address().as_ptr<PageDirectoryEntry>();

        auto user_page_directory_index = TRY(virtual_address_to_page_directory_index(virtual_address));
        auto page_directory_entry = &pd[user_page_directory_index];

        if (!page_directory_entry->present) {
            auto page_table_physical_address = TRY(allocate_physical_page());
            page_directory_entry->present = 1;
            page_directory_entry->writeable = 1;
            page_directory_entry->user_access = 1;
            page_directory_entry->physical_address = page_table_physical_address.as_address() >> 12;
        }

        auto page_table_index = TRY(virtual_address_to_page_table_index(virtual_address));
        auto page_table = page_directory_entry->get_physical_address().as_ptr<PageTableEntry>();

        auto page = &page_table[page_table_index];
        if (page->present) {
            println("FATAL: Page already allocated for {} to {}", virtual_address.as_address(),
                    page->get_physical_address().as_address());
            while (true) {
            } // FIXME: do something useful here
        }

        page->present = 1;
        page->writeable = write ? 1 : 0;
        page->execution_disabled = execute ? 0 : 1;
        page->user_access = 1;
        page->physical_address = physical_address.as_address() >> 12;
        invalidate_tlb(virtual_address);
        return {};
    }

    Result<void, MemoryManagerError> MemoryManager::set_user_directory(PhysicalAddress address) {
        m_pml4[0].present = 1;
        m_pml4[0].writeable = 1;
        m_pml4[0].user_access = 1;
        m_pml4[0].physical_address = address.as_address() >> 12;
        invalidate_entire_tlb();
        return {};
    }

    Result<VirtualAddress, MemoryManagerError> MemoryManager::allocate_user_pages(
        PhysicalAddress pdpt, const u64 virtual_base, const size_t page_count, const bool write, const bool execute) {
        PhysicalAddress start_address{};

        for (int i = 0; i < page_count; i++) {
            // Calculate the virtual address of this page
            auto page_virtual_address = VirtualAddress(virtual_base + (i * 0x1000)); // TODO: extract out the page size

            // Allocate a page
            auto page = TRY(allocate_physical_page());
            if (i == 0) {
                start_address = page;
            }

            // Map the page into user space
            map_user_page(pdpt, page, page_virtual_address, write, execute);
        }

        // Return the kernel mapping of this user-space block
        return start_address.as_virtual_address();
    }

    /**
     * Iterate through a section of virtual memory and set the read, write, and execute flags for these pages
     *
     * @param address
     * @param page_count
     * @param read
     * @param write
     * @param execute
     * @return
     */
    Result<void, MemoryManagerError> MemoryManager::set_page_flags(const VirtualAddress address,
                                                                   const size_t page_count, const bool write,
                                                                   const bool execute) {
        for (auto i = 0; i < page_count; i++) {
            const auto virtual_address = address.offset(i * 0x1000);

            const auto pte = TRY(get_kernel_page_table_entry(virtual_address));

            pte->writeable = write ? 1 : 0;
            pte->execution_disabled = execute ? 0 : 1;

            invalidate_tlb(virtual_address);
        }
        return {};
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

    void MemoryManager::unmap_identity_mapping() const {
        m_pml4[0].present = 0;
        m_pml4[0].writeable = 0;
        m_pml4[0].physical_address = 0;
        invalidate_entire_tlb();
    }

    Result<VirtualAddress, MemoryManagerError> VirtualAddressSpace::take_page() {
        if (m_free_pages == 0) {
            return MemoryManagerError(MemoryManagerError::Kind::GenericError);
        }

        const auto result = m_next_address;
        m_next_address = m_next_address.offset(Page);
        m_free_pages--;
        return result;
    }

    Result<VirtualAddress, MemoryManagerError> VirtualAddressSpace::take_pages(size_t page_count) {
        const auto virtual_address = TRY(take_page());
        for (int i = 1; i < page_count; i++) {
            TRY(take_page());
        }
        return virtual_address;
    }
} // namespace Kernel
