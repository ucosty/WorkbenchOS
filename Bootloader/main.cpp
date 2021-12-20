// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <ConsleIO.h>
#include <EFI/Efi.h>
#include <EFI/EfiWrapper.h>
#include <ELF.h>
#include <Try.h>
#include <UnbufferedConsole.h>
#include <cstring.h>

EFI::Raw::SimpleTextOutputProtocol *console_out;
extern "C" void putchar(char c) {
    wchar_t output[2] = {static_cast<wchar_t>(c), 0};
    console_out->output_string(console_out, output);
}

[[noreturn]] void panic(const char *message) {
    println(message);
    while (true) {};
}

#define ASSERT(expression)                                               \
    ({                                                                   \
        if (!(expression)) {                                             \
            printf("Assertion failed at %s:%d\r\n", __FILE__, __LINE__); \
            panic("Halting");                                            \
        }                                                                \
    })

Result<EFI::GraphicsOutputProtocol> init_graphics(EFI::BootServices *boot_services) {
    auto graphics_output = TRY(boot_services->locate_protocol<EFI::GraphicsOutputProtocol>());
    auto mode = graphics_output.mode()->mode;
    auto info = TRY(graphics_output.query_mode(mode));
    for (auto i = 0u; i < graphics_output.mode()->max_mode; i++) {
        info = TRY(graphics_output.query_mode(i));
        if (info->PixelFormat != EFI::Raw::PixelRedGreenBlueReserved8BitPerColor && info->PixelFormat != EFI::Raw::PixelBlueGreenRedReserved8BitPerColor) {
            continue;
        }
        if (info->HorizontalResolution == 1280 && info->VerticalResolution == 1024) {
            mode = i;
            break;
        }
    }
    graphics_output.set_mode(mode);
    return graphics_output;
}

Result<void> init(EFI::Raw::Handle image_handle, EFI::Raw::SystemTable *system_table) {
    auto boot_services = EFI::BootServices(system_table->boot_services);

    // Disable watchdog timer
    boot_services.set_watchdog_timer(0, 0, 0, nullptr);

    // Set up the boot-time console
    console_out = system_table->console_out;

    auto gfx = TRY(init_graphics(&boot_services));
    memset((char *) gfx.mode()->framebuffer_base, 0x88, 1280 * 1024 * 4);

    // All the bits needed to load the kernel from disk
    auto loaded_image = TRY(boot_services.handle_protocol<EFI::LoadedImageProtocol>(image_handle));
    auto file_system = TRY(boot_services.handle_protocol<EFI::SimpleFileSystemProtcol>(loaded_image.device_handle()));
    auto root = TRY(file_system.open_volume());
    auto kernel = TRY(root.open(L"kernel.elf", EFI_FILE_MODE_READ, EFI_FILE_MODE_READ));
    auto kernel_info = TRY(kernel.get_info<EFI::FileInfo>());
    auto kernel_bytes = TRY(kernel.read_bytes(kernel_info.file_size()));

    // Let's poke at the ELF elf_header
    auto elf_header = static_cast<ELF::Elf64_Ehdr *>(kernel_bytes);
    if (elf_header->ei_magic != ELF_MAGIC) {
        panic("Kernel.elf: invalid ELF magic bytes");
    }

    auto program_headers = (ELF::Elf64_Phdr *) ((char *) kernel_bytes + elf_header->e_phoff);
    auto get_total_pages = [](ELF::Elf64_Ehdr *elf_header, ELF::Elf64_Phdr *program_headers) -> uint64_t {
        auto total_pages = 0ULL;
        for (int i = 0; i < elf_header->e_phnum; i++) {
            auto program_header = &program_headers[i];
            if (program_header->p_type != PT_LOAD)
                continue;
            total_pages += (program_header->p_memsz + 0x1000 - 1) / 0x1000;
        }
        return total_pages;
    };

    auto total_pages = get_total_pages(elf_header, program_headers);
    auto kernel_base_addr = TRY(boot_services.allocate_pages(EFI::Raw::AllocateType::EFI_ALLOCATE_ANY_PAGES, EFI::Raw::MemoryType::EFI_LOADER_CODE, total_pages));

    // Convert virtual addresses to kernel physical addresses
    auto kernel_virtual_to_physical = [](uint64_t physical_base, uint64_t virtual_address) {
        static const uint64_t kernel_virtual_base = 0xffffffff80000000ull;
        return physical_base + (virtual_address - kernel_virtual_base);
    };

    // Load the program headers into memory
    for (int i = 0; i < elf_header->e_phnum; i++) {
        auto program_header = &program_headers[i];
        if (program_header->p_type != PT_LOAD) {
            continue;
        }

        auto load_pages = (program_header->p_memsz + 0x1000 - 1) / 0x1000;
        total_pages += load_pages;

        auto physical_address = kernel_virtual_to_physical(kernel_base_addr, program_header->p_vaddr);
        auto load_address = (void *) ((char *) kernel_bytes + program_header->p_offset);
        memset((char *) physical_address, 0, load_pages * 0x1000);
        memcpy((void *) physical_address, load_address, program_header->p_filesz);
    }

    // Get the system memory map
    uint64_t memory_map_size = 0;
    uint64_t memory_map_key = 0;
    uint64_t memory_map_descriptor_size = 0;
    uint32_t memory_map_descriptor_version = 0;
    EFI::Raw::MemoryDescriptor *memory_descriptor;
    boot_services.get_memory_map(&memory_map_size, nullptr, &memory_map_key, &memory_map_descriptor_size, &memory_map_descriptor_version);
    ASSERT(memory_map_size != 0);
    TRY(boot_services.allocate_pool(EFI::Raw::MemoryType::EFI_LOADER_DATA, memory_map_size, (void **) &memory_descriptor));
    TRY(boot_services.get_memory_map(&memory_map_size, memory_descriptor, &memory_map_key, &memory_map_descriptor_size, &memory_map_descriptor_version));

    // Exit boot services mode
    TRY(boot_services.exit_boot_services(image_handle, memory_map_key));

    // Jump into the kernel main function
    auto entrypoint = kernel_virtual_to_physical(kernel_base_addr, elf_header->e_entry);
    auto kernel_main = reinterpret_cast<void (*)(uint64_t)>(entrypoint);
    kernel_main(gfx.mode()->framebuffer_base);
    return {};
}

extern "C" [[noreturn]] EFI::Raw::Status efi_main(EFI::Raw::Handle *image_handle, EFI::Raw::SystemTable *system_table) {
    auto response = init(image_handle, system_table);
    if (response.is_error()) {
        auto error_code = response.get_error().get_code();
        printf("Got error: %s (%d)\r\n", EFI::status_code(error_code), error_code);
    }
    panic("You should not be here");
}