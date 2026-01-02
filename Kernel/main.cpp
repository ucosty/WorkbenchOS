// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Devices/PS2Keyboard.h"
#include "LibELF/ELF.h"
#include "Platform/x86_64/PIC.h"
#include <ACPI/ACPI.h>
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
#include <Devices/VirtioBlockDevice.h>

#include "Filesystems/Fat32.h"
#include "Process/ProcessManager.h"

using namespace Kernel;

static constexpr size_t gdt_descriptors = 7;
alignas(8) DescriptorTablePointer gdt_pointer{};
alignas(8) u64 segments[gdt_descriptors] = {
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
    0, 0
};

alignas(16) TSS tss0;

typedef void (*constructor_function)();

extern constructor_function __init_array_start[];
extern constructor_function __init_array_end[];

Console g_console;

void hexdump(const u8 *buffer, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        auto value = static_cast<u32>(buffer[i]);
        print("{:02x} ", value);

        if ((i + 1) % 16 == 0) {
            print(" | ");
            for (int y = 15; y >= 0; y--) {
                if (auto character = buffer[i - y]; character >= 32 && character < 127) {
                    print("{:c} ", character);
                } else {
                    print(". ");
                }
            }
            println("");
        }
    }
}

u64 divide_rounded_up(const u64 value, const u64 divisor) {
    return (value + (divisor - 1)) / divisor;
}

u64 bytes_to_pages(const u64 bytes) {
    return divide_rounded_up(bytes, 0x1000);
}

Result<void> jump_usermode(VirtualAddress address) {
    return {};
}

uint8_t buffer[64 * 1024] = {};

uint8_t stack1[4096] = {};
uint8_t stack2[4096] = {};
auto fat_volume = Fat32{};

/*
 *   - read the elf header
 *   - iterate through each loadable section
 *   - create a physical memory backing for that section
 *   - copy the elf data into that memory area
 *   - map the area into the user process
 */
Result<void> load_elf(const char *filename, Process *process) {
    const auto memory_manager = &MemoryManager::get_instance();

    // Fetch the ELF header
    const auto file_size = TRY(fat_volume.get_file_size(filename));
    TRY(fat_volume.read_file(filename, buffer, file_size));

    const auto header = reinterpret_cast<ELF::Elf64_Ehdr *>(buffer);
    if (header->ei_magic != ELF_MAGIC) {
        println("Invalid ELF magic, found {:#x}", header->ei_magic);
        return Error::from_code(1);
    }

    const auto program_headers = reinterpret_cast<ELF::Elf64_Phdr *>(&buffer[header->e_phoff]);

    // Load the program headers into memory
    for (int i = 0; i < header->e_phnum; i++) {
        const auto program_header = &program_headers[i];
        if (program_header->p_type != PT_LOAD) {
            continue;
        }

        const auto load_pages = bytes_to_pages(program_header->p_memsz);
        const auto source = static_cast<void *>(&buffer[program_header->p_offset]);
        auto destination = TRY_INTO(Error, memory_manager->allocate_user_pages(process->get_page_directory(), program_header->p_vaddr, load_pages, (program_header->p_flags & 0x2) >> 1, program_header->p_flags & 0x1));

        memset(reinterpret_cast<char *>(destination.as_ptr()), 0, load_pages * Page);
        memcpy(destination.as_ptr(), source, program_header->p_filesz);
    }

    println("Loaded {} with {} sections", filename, header->e_phnum);

    return {};
}

Result<void> run_executable(const char *filename) {
    const auto process_manager = &ProcessManager::get_instance();
    // const auto memory_manager = &MemoryManager::get_instance();

    // Create a process
    const auto process = TRY(process_manager->create_process());

    // Load the executable
    TRY(load_elf(filename, process));

    // memory_manager->map_user_page(page_directory, );

    return {};
}


extern "C" [[noreturn]] void kernel_stage2(const BootState &boot_state) {
    for (auto init = __init_array_start; init < __init_array_end; init++) {
        (*init)();
    }

    PIC::disable();
    Processor::load_task_register(0x28);

    auto &ivt = InterruptVectorTable::get_instance();
    ivt.initialise();

    auto &memory_manager = MemoryManager::get_instance();
    memory_manager.init(boot_state);
    TRY_PANIC(g_malloc_heap.initialise());

    auto framebuffer = LinearFramebuffer(boot_state.kernel_address_space.framebuffer.virtual_base,
                                         boot_state.framebuffer.width, boot_state.framebuffer.height);
    framebuffer.rect(50, 50, 100, 100, 0x4455aa, true);
    g_console.initialise(boot_state.kernel_address_space.framebuffer.virtual_base, boot_state.framebuffer.width,
                         boot_state.framebuffer.height);
    g_console.println("\n\nKernel loaded!");

    auto &acpi = ACPI::get_instance();
    TRY_PANIC(acpi.initialise(PhysicalAddress(boot_state.acpi_root_table_address)));
    TRY_PANIC(acpi.find_devices());

    APIC apic;
    TRY_PANIC(apic.initialise());

    auto pci = PCI();
    TRY_PANIC(pci.initialise());

    auto device = TRY_PANIC(VirtioBlockDevice::detect_and_init(&pci));
    TRY_PANIC(fat_volume.mount(&device, 0));

    TRY_PANIC(run_executable("/wbinit"));

    while (true) {
    }

    // Multiprocessor code
    // acpi.start_application_processors();
    //
    // auto stack = TRY_PANIC(memory_manager.allocate_kernel_heap_page());
    // tss0.rsp0 = stack.as_address() + Page;
    // Processor::load_task_register(0x28);

    Processor::halt();
}

extern "C" [[noreturn]] EFICALL void kernel_main(u64 boot_state_address) {
    gdt_pointer.address = reinterpret_cast<u64>(&segments);
    gdt_pointer.limit = sizeof(SegmentDescriptor) * gdt_descriptors;

    auto tss0_address = reinterpret_cast<u64>(&tss0);
    TSSDescriptor tss_descriptor_0{
        .segment_limit = sizeof(TSS) - 1,
        .base_low = tss0_address & 0xffff,
        .base_low_middle = (tss0_address >> 16) & 0xff,
        .segment_type = 9,
        .privilege_level = 0,
        .present = 1,
        .granularity = 0,
        .base_high_middle = (tss0_address >> 24) & 0xff,
        .base_high = (u32) (tss0_address >> 32),
    };

    // configure the ist
    tss0.ist1 = reinterpret_cast<uint64_t>(&stack1) + 4096;
    tss0.ist2 = reinterpret_cast<uint64_t>(&stack2) + 4096;

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
        "pushq $0x08\n" // Push CS
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
        "pushq $0x08\n" // Push CS
        "pushq $ap_stage2\n"// Push RIP
        "lretq\n");
    Processor::halt();
}
