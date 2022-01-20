// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "ACPI/ACPI.h"
#include "APIC.h"
#include "Debugging.h"
#include "Heap/Kmalloc.h"
#include "Interfaces/PCI.h"
#include "InterruptVectorTable.h"
#include "Memory/MemoryManager.h"
#include "Process/ProcessManager.h"
#include "Processor.h"
#include <BootState.h>
#include <ConsoleIO.h>
#include <Descriptors.h>
#include <LinearFramebuffer.h>
#include <Types.h>

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

extern "C" [[noreturn]] void kernel_stage2(const BootState &boot_state) {
    // TODO: Ensure C++ constructors are run
    // TODO: SystemDescriptorTables initial support to find devices
    auto &ivt = InterruptVectorTable::get_instance();
    ivt.initialise();

    auto &memory_manager = MemoryManager::get_instance();
    memory_manager.init(boot_state);
    TRY_PANIC(g_malloc_heap.initialise());

    auto &acpi = ACPI::get_instance();
    TRY_PANIC(acpi.initialise(PhysicalAddress(boot_state.acpi_root_table_address)));

    APIC apic;
    TRY_PANIC(apic.initialise());
    acpi.start_application_processors();

    auto stack = TRY_PANIC(memory_manager.allocate_kernel_heap_page());
    tss0.rsp0 = stack.as_address() + Page;
    Processor::load_task_register(0x28);

    auto pci = PCI();
    TRY_PANIC(pci.initialise());

//    auto &process_manager = ProcessManager::get_instance();
//    TRY_PANIC(process_manager.initialise());
//    TRY_PANIC(process_manager.create_process());
//    Processor::interrupt();


    auto framebuffer = LinearFramebuffer(boot_state.kernel_address_space.framebuffer.virtual_base, 1280, 1024);
    framebuffer.rect(50, 50, 100, 100, 0x4455aa, true);

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
