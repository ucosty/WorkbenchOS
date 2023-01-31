// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Devices/PS2Keyboard.h"
#include "LibELF/ELF.h"
#include <ACPI/ACPI.h>
#include <ACPI/ACPICA.h>
#include <APIC.h>
#include <BootConsole/Console.h>
#include <BootState.h>
#include <Bus/PCI.h>
#include <Debugging.h>
#include <Descriptors.h>
#include <Devices/MemoryBlockDevice.h>
#include <Filesystems/RamdiskFS.h>
#include <Heap/Kmalloc.h>
#include <InterruptVectorTable.h>
#include <LibStd/Types.h>
#include <LinearFramebuffer.h>
#include <Memory/MemoryManager.h>
#include <Processor.h>

using namespace Kernel;

static constexpr size_t gdt_descriptors = 7;
alignas(8) DescriptorTablePointer gdt_pointer{};
alignas(8) uint64_t segments[gdt_descriptors] = {
    // Null segment
    SegmentDescriptor{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}.descriptor(),
    // Kernel Code Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_EXECUTE, 1, 0, 1, 0, 0, 1, 0, 0, 0}.descriptor(),
    // Kernel Data Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_READ_WRITE, 1, 0, 1, 0, 0, 0, 0, 0, 0}.descriptor(),
    // User Code Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_EXECUTE, 1, 3, 1, 0, 0, 1, 0, 0, 0}.descriptor(),
    // User Data Segment
    SegmentDescriptor{0, 0, 0, SEGMENT_READ_WRITE, 1, 3, 1, 0, 0, 0, 0, 0, 0}.descriptor(),
    // TSS 0
    0, 0};

alignas(16) TSS tss0;

typedef void (*constructor_function)();
extern constructor_function __init_array_start[];
extern constructor_function __init_array_end[];

Console g_console;

void hexdump(uint8_t *buffer, size_t size) {
    for(auto i = 0; i < size; i++) {
        if((i + 1) % 16 == 0) {
            printf(" | ");
            for(int y = 15; y >= 0; y--) {
                auto character = buffer[i - y];
                if(character >= 32 && character < 127) {
                    printf("%c ", buffer[i - y]);
                } else {
                    printf(". ");
                }
            }
            printf("\n");
        }

        auto value = static_cast<uint32_t>(buffer[i]);
        if(value <= 0x0f) {
            printf("0%x ", value);
        } else {
            printf("%x ", value);
        }
    }
}

uint64_t divide_rounded_up(uint64_t value, uint64_t divisor) {
    return (value + (divisor - 1)) / divisor;
}

uint64_t bytes_to_pages(uint64_t bytes) {
    return divide_rounded_up(bytes, 0x1000);
}

Result<VirtualAddress> load_executable(uint8_t *buffer) {
    auto elf_header = reinterpret_cast<ELF::Elf64_Ehdr *>(buffer);
    if (elf_header->ei_magic != ELF_MAGIC) {
        return Error::with_message("Invalid ELF magic bytes"_sv);
    }

    auto program_headers = reinterpret_cast<ELF::Elf64_Phdr *>(buffer + elf_header->e_phoff);
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

    auto pages = get_total_pages(elf_header, program_headers);

    println("Loading ELF file...");
    println("{} pages", pages);

    auto &memory_manager = MemoryManager::get_instance();
    auto directory = TRY(memory_manager.create_user_mode_directory());
    memory_manager.set_user_directory(directory);

    for (int i = 0; i < elf_header->e_phnum; i++) {
        auto program_header = &program_headers[i];
        if (program_header->p_type != PT_LOAD) {
            continue;
        }

//        println("Loading section: type = {}, vaddr = {}, size = {}", program_header->p_type, (void *) program_header->p_vaddr, program_header->p_memsz);

        auto load_pages = bytes_to_pages(program_header->p_memsz);
        TRY(directory.allocate_pages_with_virtual_base(program_header->p_vaddr, load_pages));

        // copy section into directory pages




//        auto physical_address = kernel_virtual_to_physical(kernel_physical_base, program_header->p_vaddr);
//        auto load_address = (void *) ((char *) kernel_bytes + program_header->p_offset);
//        memset((char *) physical_address, 0, load_pages * 0x1000);
//        memcpy((void *) physical_address, load_address, program_header->p_filesz);
    }

//    auto &memory_manager = MemoryManager::get_instance();
//    for(int i = 0; i < pages; i++) {
//        auto physical_page = TRY(memory_manager.allocate_physical_page());
//    }


    return VirtualAddress(nullptr);
}

Result<void> jump_usermode(VirtualAddress address) {
    return {};
}

extern "C" [[noreturn]] void kernel_stage2(const BootState &boot_state) {
    for (auto init = __init_array_start; init < __init_array_end; init++) {
        (*init)();
    }

    // Disable the PIC
    asm volatile("mov $0xff, %al\n"
                 "outb %al, $0xa1\n"
                 "outb %al, $0x21\n");

    // TODO: Ensure C++ constructors are run
    // TODO: SystemDescriptorTables initial support to find devices
    auto &ivt = InterruptVectorTable::get_instance();
    ivt.initialise();

    auto &memory_manager = MemoryManager::get_instance();
    memory_manager.init(boot_state);
    TRY_PANIC(g_malloc_heap.initialise());

    auto framebuffer = LinearFramebuffer(boot_state.kernel_address_space.framebuffer.virtual_base, boot_state.framebuffer.width, boot_state.framebuffer.height);
    framebuffer.rect(50, 50, 100, 100, 0x4455aa, true);
    g_console.initialise(boot_state.kernel_address_space.framebuffer.virtual_base, boot_state.framebuffer.width, boot_state.framebuffer.height);
    g_console.println("\n\nKernel loaded!");

    auto &acpi = ACPI::get_instance();
    TRY_PANIC(acpi.initialise(PhysicalAddress(boot_state.acpi_root_table_address)));
    TRY_PANIC(acpi.find_devices());

    APIC apic;
    TRY_PANIC(apic.initialise());
    acpi.start_application_processors();

    auto stack = TRY_PANIC(memory_manager.allocate_kernel_heap_page());
    tss0.rsp0 = stack.as_address() + Page;
    Processor::load_task_register(0x28);

//    auto pci = PCI();
//    TRY_PANIC(pci.initialise());
//
    auto ram_block_device = MemoryBlockDevice(boot_state.ramdisk.address.as_virtual_address(), boot_state.ramdisk.size);
    auto ramdisk_fs = RamdiskFS::Filesystem(&ram_block_device);
    TRY_PANIC(ramdisk_fs.init());

    auto file = TRY_PANIC(ramdisk_fs.open("test"));
    auto buffer = new uint8_t[file.size()];
    TRY_PANIC(file.read(buffer, file.size()));

    // load executable into new process memory space
    // create address space
    // jump into new process in user mode
    auto init_process = TRY_PANIC(load_executable(buffer));
    TRY_PANIC(jump_usermode(init_process));

    Processor::halt();
}

extern "C" [[noreturn]] EFICALL void kernel_main(uint64_t boot_state_address) {
    gdt_pointer.address = reinterpret_cast<uint64_t>(&segments);
    gdt_pointer.limit = sizeof(SegmentDescriptor) * gdt_descriptors;

    auto tss0_address = reinterpret_cast<uint64_t>(&tss0);
    TSSDescriptor tss_descriptor_0{
        .segment_limit = sizeof(TSS) - 1,
        .base_low = tss0_address & 0xffff,
        .base_low_middle = (tss0_address >> 16) & 0xff,
        .segment_type = 9,
        .privilege_level = 0,
        .present = 1,
        .granularity = 0,
        .base_high_middle = (tss0_address >> 24) & 0xff,
        .base_high = (uint32_t) (tss0_address >> 32),
    };

    segments[5] = tss_descriptor_0.low;
    segments[6] = tss_descriptor_0.high;
    asm volatile("lgdt gdt_pointer\n"
                 "mov %%rax, %%rdi\n"// Boot state block
                 "mov $0x10, %%rbx\n"
                 "mov %%rbx, %%ss\n"
                 "mov %%rbx, %%ds\n"
                 "mov %%rbx, %%es\n"
                 "mov %%rbx, %%fs\n"
                 "mov %%rbx, %%gs\n"
                 "movq $0x0, %%rbp\n"
                 "pushq $0x08\n"         // Push CS
                 "pushq $kernel_stage2\n"// Push RIP
                 "lretq\n"
                 : /* no output */
                 : "a"(boot_state_address));
    Processor::halt();
}

extern "C" [[noreturn]] void ap_stage2() {
    ACPI::set_booted();
    auto cpu = Processor::local_apic_id();
    Processor::halt();
}

extern "C" [[noreturn]] void ap_main() {
    asm volatile("lgdt gdt_pointer\n"
                 "mov $0x10, %rbx\n"
                 "mov %rbx, %ss\n"
                 "mov %rbx, %ds\n"
                 "mov %rbx, %es\n"
                 "mov %rbx, %fs\n"
                 "mov %rbx, %gs\n"
                 "movq $0x0, %rbp\n"
                 "pushq $0x08\n"     // Push CS
                 "pushq $ap_stage2\n"// Push RIP
                 "lretq\n");
    Processor::halt();
}
