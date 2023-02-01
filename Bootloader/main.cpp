// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <ACPI/Tables.h>
#include <BootState.h>
//#include <ConsoleIO.h>
#include <EFI/Efi.h>
#include <EFI/EfiWrapper.h>
#include <LibELF/ELF.h>
#include <LibStd/Try.h>
#include <PageStructures.h>
#include <UnbufferedConsole.h>
#include "BootConsole.h"
#include <LibStd/CString.h>

using namespace Std;

namespace Std {
String::~String() {
}

String::String(const char *characters) {
}

String::String(const StringView &sv) : m_length(sv.m_length) {
}

String::String(const char *characters, size_t length) : m_length(length) {
}

String::String(String const &other) {
}
}// namespace Std

constexpr uint64_t GUARD_PAGE = 1;
constexpr uint64_t kernel_virtual_base = 0xffffffff80000000;

BootConsole g_boot_console;

void inline outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

void debug_putstring(const char *string) {
    g_boot_console.print(string);
    while (*string != '\0') {
        outb(0xe9, *string);
        string++;
    }
}

void debug_putchar(char c) {
    outb(0xe9, c);
}

[[noreturn]] void panic(const char *message) {
    g_boot_console.println(message);
    while (true) {};
}

uint64_t divide_rounded_up(uint64_t value, uint64_t divisor) {
    return (value + (divisor - 1)) / divisor;
}

/**
 * Get the number of memory pages required to fit the number of bytes
 * provided, rounded up to the nearest page.
 * @param bytes to fit
 * @return page count
 */
uint64_t bytes_to_pages(uint64_t bytes) {
    return divide_rounded_up(bytes, 0x1000);
}

uint64_t next_virtual_mapping(VirtualMapping &last) {
    return last.virtual_base + last.size + 0x1000;
}

uint64_t get_efi_preserved_page_count(EFI::MemoryMap &memory_map) {
    uint64_t total = 0;
    for (auto &descriptor: memory_map) {
        switch (descriptor.type) {
            case EFI::EFI_RUNTIME_SERVICES_DATA:
            case EFI::EFI_RUNTIME_SERVICES_CODE:
            case EFI::EFI_ACPI_RECLAIM_MEMORY:
            case EFI::EFI_ACPI_MEMORY_NVS:
            case EFI::EFI_PAL_CODE:
                total += descriptor.number_of_pages;
                break;
        }
    }
    return total;
}

uint64_t get_physical_memory_extent(EFI::MemoryMap &memory_map) {
    uint64_t high_water_mark = 0;
    for (auto &descriptor: memory_map) {
        switch (descriptor.type) {
            case EFI::EFI_UNUSABLE_MEMORY:
                break;
            default:
                high_water_mark = descriptor.physical_start + (descriptor.number_of_pages * Page);
        }
    }
    return high_water_mark;
}

size_t bitmap_required_size_in_bytes(uint64_t pool_size, uint64_t unit_size) {
    uint64_t pool_size_in_bits = (pool_size / unit_size) * 2;
    uint64_t pool_size_in_bytes = pool_size_in_bits / 8;
    return pool_size_in_bytes;
}

uint64_t get_pagetable_size(uint64_t pages) {
    // For every 2MiB of pages, we need a Page Table
    auto pages_bytes = pages * Page;
    auto page_tables = divide_rounded_up(pages_bytes, 2 * MiB);

    // For every 1GiB of pages, we need a Page Directory
    auto page_directories = divide_rounded_up(pages_bytes, 1 * GiB);

    // For every 512GiB of pages, we need a PDP
    auto page_directory_pointers = divide_rounded_up(pages_bytes, 512 * GiB);

    // We need an additional PDPT for the temporary identity-mapping of memory
    page_directory_pointers++;

    // We always need a single PML4 page
    auto pml4 = 1;

    return pml4 + page_directory_pointers + page_directories + page_tables;
}

Result<EFI::GraphicsOutputProtocol> init_graphics(EFI::BootServices *boot_services) {
    auto graphics_output = TRY(boot_services->locate_protocol<EFI::GraphicsOutputProtocol>());
    auto mode = graphics_output.mode()->mode;
    auto info = TRY(graphics_output.query_mode(mode));
    for (auto i = 0u; i < graphics_output.mode()->max_mode; i++) {
        info = TRY(graphics_output.query_mode(i));
        if (info->pixel_format != EFI::Raw::PixelRedGreenBlueReserved8BitPerColor && info->pixel_format != EFI::Raw::PixelBlueGreenRedReserved8BitPerColor) {
            continue;
        }
        if (info->width == 1280 && info->height == 1024) {
            mode = i;
            break;
        }
    }
    graphics_output.set_mode(mode);
    return graphics_output;
}

bool verify_rsdp_checksum(RootSystemDescriptionPointer *rsdp) {
    int32_t checksum = 0;
    auto *bytes = (uint8_t *) rsdp;
    for (int i = 0; i < 20; i++) {
        checksum += bytes[i];
    }
    return (checksum & 0xFF) == 0;
}

Result<uint64_t> find_acpi_root_table(EFI::Raw::SystemTable *system_table) {
    for (int i = 0; i < system_table->NumberOfTableEntries; i++) {
        auto table = system_table->ConfigurationTable[i];
        if (!table.vendor_guid.equals(EFI::acpi_root_table_guid))
            continue;

        auto rsdp = reinterpret_cast<RootSystemDescriptionPointer *>(table.vendor_table);
        if (rsdp->signature != RSDP_SIGNATURE)
            continue;

        if (!verify_rsdp_checksum(rsdp))
            continue;

        return reinterpret_cast<uint64_t>(table.vendor_table);
    }
    println("PANIC: Could not find ACPI configuration table!");
    return Error::from_code(1);
}

Result<void> init(EFI::Raw::Handle image_handle, EFI::Raw::SystemTable *system_table) {
    auto boot_services = EFI::BootServices(system_table->boot_services);

    // Disable watchdog timer
    boot_services.set_watchdog_timer(0, 0, 0, nullptr);

    auto gfx = TRY(init_graphics(&boot_services));
    memset((char *) gfx.mode()->framebuffer_base, 0x77, 1280 * 1024 * 4);

    g_boot_console.initialise(reinterpret_cast<uint32_t *>(gfx.mode()->framebuffer_base), 1280, 1024);
    g_boot_console.println("WorkbenchOS is booting...");

    // All the bits needed to load the kernel from disk
    auto loaded_image = TRY(boot_services.handle_protocol<EFI::LoadedImageProtocol>(image_handle));
    auto file_system = TRY(boot_services.handle_protocol<EFI::SimpleFileSystemProtcol>(loaded_image.device_handle()));
    auto root = TRY(file_system.open_volume());
    auto kernel = TRY(root.open(L"kernel.elf", EFI_FILE_MODE_READ, EFI_FILE_MODE_READ));

    auto kernel_info = TRY(kernel.get_info<EFI::FileInfo>());
    auto kernel_bytes = TRY(kernel.read_bytes(kernel_info.file_size()));

    // Load the init ramdisk
    auto ramdisk = TRY(root.open(L"ramdisk.bin", EFI_FILE_MODE_READ, EFI_FILE_MODE_READ));
    auto ramdisk_info = TRY(ramdisk.get_info<EFI::FileInfo>());
    auto ramdisk_bytes = TRY(ramdisk.read_bytes(ramdisk_info.file_size()));

    // Let's poke at the ELF elf_header
    auto elf_header = static_cast<ELF::Elf64_Ehdr *>(kernel_bytes);
    if (elf_header->ei_magic != ELF_MAGIC) {
        panic("Kernel.elf: invalid ELF magic bytes");
    }

    auto program_headers = reinterpret_cast<ELF::Elf64_Phdr *>((char *) kernel_bytes + elf_header->e_phoff);
    auto get_total_pages = [](ELF::Elf64_Ehdr *elf_header, ELF::Elf64_Phdr *program_headers) -> uint64_t {
        auto total_pages = 0ULL;
        for (int i = 0; i < elf_header->e_phnum; i++) {
            auto program_header = &program_headers[i];
            if (program_header->p_type != PT_LOAD)
                continue;
            total_pages += bytes_to_pages(program_header->p_memsz);
        }
        return total_pages;
    };

    auto kernel_pages = get_total_pages(elf_header, program_headers);

    // Set aside space for the initial page tables

    /**
     * Memory Map
     *
     * 0x0   ------------------------------
     *  .    |                            |
     *  .    |                            |
     *  .    |                            |
     *  .    |                            |
     *  .    |  Unalllocated              |
     *  .    |                            |
     *  .    |                            |
     *  .    |                            |
     *  .    |                            |
     * -2GiB ------------------------------
     *  .    |                            |
     *  .    | Kernel                     |
     *  .    |                            |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    | Boot State Block           |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    |                            |
     *  .    | Initial Pages              |
     *  .    |                            |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    |                            |
     *  .    | Memory Map                 |
     *  .    |                            |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    |                            |
     *  .    | Framebuffer                |
     *  .    |                            |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    |                            |
     *  .    | Kernel Stack               |
     *  .    |                            |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    |                            |
     *  .    | Frame Allocator Bitmap     |
     *  .    |                            |
     *       ------------------------------
     *       + Guard Page                 +
     *       ------------------------------
     *  .    |                            |
     *  .    |                            |
     *  .    | Unallocated (Kernel Heap)  |
     *  .    |                            |
     *  .    |                            |
     *       ------------------------------
     */
    auto memory_map = TRY(boot_services.get_memory_map());
    //    TRY(memory_map.sanity_check());

    // Allocate for the initial frame allocator
    auto memory_size = get_physical_memory_extent(memory_map);
    auto bitmap_size_in_bytes = bitmap_required_size_in_bytes(memory_size, Page);
    auto bitmap_size_in_pages = divide_rounded_up(bitmap_size_in_bytes, Page);
    auto bitmap_physical_base = TRY(boot_services.allocate_pages(EFI::Raw::AllocateType::EFI_ALLOCATE_ANY_PAGES, EFI::MemoryType::EFI_LOADER_DATA, bitmap_size_in_pages));
    memset((char *) bitmap_physical_base, 0, bitmap_size_in_pages * Page);

    // Start with the kernel pages
    auto pages = kernel_pages + GUARD_PAGE;

    // Add the memory map
    pages += bytes_to_pages(memory_map.m_size) + GUARD_PAGE;

    // Add the framebuffer
    pages += bytes_to_pages(gfx.mode()->framebuffer_size) + GUARD_PAGE;

    // Add the EFI preserved memory
    pages += get_efi_preserved_page_count(memory_map) + GUARD_PAGE;

    // Add the initial page table pages
    auto pagetable_size = get_pagetable_size(pages);
    pages += pagetable_size + GUARD_PAGE;

    // Add the frame allocator pages
    pages += bitmap_size_in_pages + GUARD_PAGE;

    // Allocate memory for the kernel
    auto kernel_physical_base = TRY(boot_services.allocate_pages(EFI::Raw::AllocateType::EFI_ALLOCATE_ANY_PAGES, EFI::MemoryType::EFI_LOADER_DATA, pages));
    memset((char *) kernel_physical_base, 0, pages * Page);

    // Allocate memory for the boot state block
    auto boot_state_page_count = bytes_to_pages(sizeof(BootState));
    auto boot_state_base = TRY(boot_services.allocate_pages(EFI::Raw::AllocateType::EFI_ALLOCATE_ANY_PAGES, EFI::MemoryType::EFI_LOADER_DATA, boot_state_page_count));
    memset((char *) boot_state_base, 0, boot_state_page_count * Page);
    auto boot_state = reinterpret_cast<BootState *>(boot_state_base);

    // Allocate memory for the kernel stack
    auto kernel_stack_page_count = 2;
    auto kernel_stack_physical_base = TRY(boot_services.allocate_pages(EFI::Raw::AllocateType::EFI_ALLOCATE_ANY_PAGES, EFI::MemoryType::EFI_LOADER_DATA, kernel_stack_page_count));
    memset((char *) kernel_stack_physical_base, 0, kernel_stack_page_count * Page);

    // Convert virtual addresses to kernel physical addresses
    auto kernel_virtual_to_physical = [](uint64_t physical_base, uint64_t virtual_address) {
        return physical_base + (virtual_address - kernel_virtual_base);
    };

    // Load the program headers into memory
    for (int i = 0; i < elf_header->e_phnum; i++) {
        auto program_header = &program_headers[i];
        if (program_header->p_type != PT_LOAD) {
            continue;
        }

        auto load_pages = bytes_to_pages(program_header->p_memsz);
        auto physical_address = kernel_virtual_to_physical(kernel_physical_base, program_header->p_vaddr);
        auto load_address = (void *) ((char *) kernel_bytes + program_header->p_offset);
        memset((char *) physical_address, 0, load_pages * 0x1000);
        memcpy((void *) physical_address, load_address, program_header->p_filesz);
    }

    // Get the system memory map
    memory_map = TRY(boot_services.get_memory_map());

    // Exit boot services mode
    TRY(boot_services.exit_boot_services(image_handle, memory_map.m_key));

    boot_state->physical_memory_size = memory_size;

    boot_state->ramdisk.address = PhysicalAddress(reinterpret_cast<uint64_t>(ramdisk_bytes));
    boot_state->ramdisk.size = ramdisk_info.file_size();

    boot_state->framebuffer.size = gfx.mode()->framebuffer_size;
    boot_state->framebuffer.width = gfx.mode()->info->width;
    boot_state->framebuffer.height = gfx.mode()->info->height;

    boot_state->kernel_address_space.base = 0xffffffff80000000;
    boot_state->kernel_address_space.size = 2 * GiB;
    boot_state->acpi_root_table_address = TRY(find_acpi_root_table(system_table));

    // Kernel memory region
    boot_state->kernel_address_space.kernel.virtual_base = 0xffffffff80000000;
    boot_state->kernel_address_space.kernel.physical_base = kernel_physical_base;
    boot_state->kernel_address_space.kernel.size = kernel_pages * Page;

    // Boot state region
    boot_state->kernel_address_space.boot_state.physical_base = boot_state_base;
    boot_state->kernel_address_space.boot_state.virtual_base = next_virtual_mapping(boot_state->kernel_address_space.kernel);
    boot_state->kernel_address_space.boot_state.size = boot_state_page_count * Page;

    // Initial pages region
    boot_state->kernel_address_space.initial_pages.physical_base = kernel_physical_base + (kernel_pages * Page);
    boot_state->kernel_address_space.initial_pages.virtual_base = next_virtual_mapping(boot_state->kernel_address_space.boot_state);
    boot_state->kernel_address_space.initial_pages.size = pagetable_size * Page;

    // Memory map region
    boot_state->kernel_address_space.memory_map.virtual_base = next_virtual_mapping(boot_state->kernel_address_space.initial_pages);
    boot_state->kernel_address_space.memory_map.size = bytes_to_pages(memory_map.m_size) * Page;
    boot_state->kernel_address_space.memory_map.physical_base = (uint64_t) memory_map.m_descriptors;

    // Framebuffer region
    boot_state->kernel_address_space.framebuffer.virtual_base = next_virtual_mapping(boot_state->kernel_address_space.memory_map);
    boot_state->kernel_address_space.framebuffer.size = bytes_to_pages(gfx.mode()->framebuffer_size) * Page;
    boot_state->kernel_address_space.framebuffer.physical_base = gfx.mode()->framebuffer_base;

    // Stack region
    boot_state->kernel_address_space.stack.virtual_base = next_virtual_mapping(boot_state->kernel_address_space.framebuffer);
    boot_state->kernel_address_space.stack.size = kernel_stack_page_count * Page;
    boot_state->kernel_address_space.stack.physical_base = kernel_stack_physical_base;

    // Initial frame allocator
    boot_state->kernel_address_space.frame_allocator.virtual_base = next_virtual_mapping(boot_state->kernel_address_space.stack);
    boot_state->kernel_address_space.frame_allocator.size = bitmap_size_in_pages * Page;
    boot_state->kernel_address_space.frame_allocator.physical_base = bitmap_physical_base;

    // Memory Map object
    boot_state->memory_map.m_size = memory_map.m_size;
    boot_state->memory_map.m_descriptor_size = memory_map.m_descriptor_size;
    boot_state->memory_map.m_descriptor_count = memory_map.m_descriptor_count;
    boot_state->memory_map.m_key = memory_map.m_key;
    boot_state->memory_map.m_descriptor_version = memory_map.m_descriptor_version;
    boot_state->memory_map.m_descriptors = reinterpret_cast<EFI::MemoryDescriptor *>(boot_state->kernel_address_space.memory_map.virtual_base);

    // Set up initial paging
    auto paging_physical_base = boot_state->kernel_address_space.initial_pages.physical_base;
    auto pml4 = (PML4Entry *) paging_physical_base;                                         // 256 TiB / 512 GiB per entry
    auto pdpt = (PageDirectoryPointerTableEntry *) (paging_physical_base + 0x1000);         // 512 GiB / 1 GiB per entry
    auto pdpt_identity = (PageDirectoryPointerTableEntry *) (paging_physical_base + 0x2000);// 512 GiB / 1 GiB per entry
    auto kernel_page_directory = (PageDirectoryEntry *) (paging_physical_base + 0x3000);    // 1 GiB / 2 MiB per entry.
    auto kernel_page_tables = (PageTableEntry *) (paging_physical_base + 0x4000);           // 2 MiB / 4 KiB per entry

    // Zeroise page structures
    memset((char *) pml4, 0, 0x1000);
    memset((char *) pdpt, 0, 0x1000);
    memset((char *) pdpt_identity, 0, 0x1000);
    memset((char *) kernel_page_directory, 0, 0x1000);
    memset((char *) kernel_page_tables, 0, 0x1000);

    boot_state->kernel_address_space.kernel_page_directory_virtual_address = boot_state->kernel_address_space.initial_pages.virtual_base + 0x3000;

    pml4[511].present = 1;
    pml4[511].writeable = 1;
    pml4[511].physical_address = ((uint64_t) pdpt) >> 12;

    // Temporarily identity-map the first 512 GiB of memory
    pml4[0].present = 1;
    pml4[0].writeable = 1;
    pml4[0].physical_address = ((uint64_t) pdpt_identity) >> 12;
    for (int i = 0; i < 512; i++) {
        pdpt_identity[i].present = 1;
        pdpt_identity[i].writeable = 1;
        pdpt_identity[i].user_access = 0;
        pdpt_identity[i].size = 1;
        pdpt_identity[i].global = 1;
        pdpt_identity[i].physical_address = (i * GiB) >> 12;
    }

    // Map 512 GiB of memory to the 511th PML4 entry (-1024 GiB to -512 GiB)
    pml4[510].present = 1;
    pml4[510].writeable = 1;
    pml4[510].physical_address = ((uint64_t) pdpt_identity) >> 12;

    pdpt[510].present = 1;
    pdpt[510].writeable = 1;
    pdpt[510].physical_address = ((uint64_t) kernel_page_directory) >> 12;

    auto page_tables_count = divide_rounded_up(pages, 512);
    for (int i = 0; i < page_tables_count; i++) {
        auto page_table_address = ((uint64_t) kernel_page_tables) + (i * 0x1000);
        kernel_page_directory[i].present = 1;
        kernel_page_directory[i].writeable = 1;
        kernel_page_directory[i].physical_address = page_table_address >> 12;
    }

    auto virtual_to_page_table = [](uint64_t virtual_address) -> uint64_t {
        auto offset_address = virtual_address - kernel_virtual_base;
        return offset_address >> 12;
    };

    auto map_address_space = [&virtual_to_page_table, &kernel_page_tables](VirtualMapping &address_space) {
        auto pages = address_space.size / 0x1000;
        for (auto i = 0ull; i < pages; i++) {
            auto page_physical = address_space.physical_base + (i * 0x1000);
            auto page_virtual = address_space.virtual_base + (i * 0x1000);
            auto page_table_entry = virtual_to_page_table(page_virtual);
            kernel_page_tables[page_table_entry].present = 1;
            kernel_page_tables[page_table_entry].writeable = 1;
            kernel_page_tables[page_table_entry].physical_address = page_physical >> 12;
        }
    };

    // Map the kernel
    map_address_space(boot_state->kernel_address_space.kernel);
    map_address_space(boot_state->kernel_address_space.boot_state);
    map_address_space(boot_state->kernel_address_space.initial_pages);
    map_address_space(boot_state->kernel_address_space.memory_map);
    map_address_space(boot_state->kernel_address_space.framebuffer);
    map_address_space(boot_state->kernel_address_space.stack);
    map_address_space(boot_state->kernel_address_space.frame_allocator);

    // Disable interrupts and load the new page tables
    auto entrypoint = elf_header->e_entry;
    asm volatile("cli");
    asm volatile("mov %%rax, %%cr3"
                 : /* no output */
                 : "a"(paging_physical_base)
                 : "memory");
    asm volatile("mov %%rax, %%rsp\n"
                 "mov %%rax, %%rbp\n"
                 "push %%rcx\n"
                 "jmp *(%%rdx)"
                 : /* no output */
                 : "a"(boot_state->kernel_address_space.stack.virtual_base + boot_state->kernel_address_space.stack.size - 0x1000),
                   "c"(boot_state->kernel_address_space.boot_state.virtual_base),
                   "d"(&entrypoint)
                 : "memory");
    return {};
}

extern "C" [[noreturn]] EFI::Raw::Status efi_main(EFI::Raw::Handle *image_handle, EFI::Raw::SystemTable *system_table) {
    auto response = init(image_handle, system_table);
    if (response.is_error()) {
        println("Got error: {}", response.get_error().get_message());
        //        g_boot_console.println(response.get_error().get_message());
    }
    panic("You should not be here");
}