// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "ACPI/DescriptorTables.h"
#include "Debugging.h"
#include "Exceptions.h"
#include "Heap/Kmalloc.h"
#include "Memory/MemoryManager.h"
#include <BootState.h>
#include <Descriptors.h>
#include <LinearFramebuffer.h>
#include <Types.h>

extern "C" [[noreturn]] void kernel_stage2(const BootState &boot_state) {
    // TODO: Ensure C++ constructors are run
    // TODO: ACPI initial support to find devices
    // TODO: xAPIC timer driver
    configure_exceptions();

    auto &memory_manager = Kernel::MemoryManager::get_instance();
    memory_manager.init(boot_state);
    TRY_PANIC(g_malloc_heap.initialise());
    TRY_PANIC(g_acpi.initialise(PhysicalAddress(boot_state.acpi_root_table_address)));

    auto framebuffer = LinearFramebuffer(boot_state.kernel_address_space.framebuffer.virtual_base, 1280, 1024);
    framebuffer.rect(50, 50, 100, 100, 0x4455aa, true);
    while (true) {}
}

static constexpr size_t gdt_descriptors = 5;
alignas(8) DescriptorTablePointer gdt_pointer{};
alignas(8) SegmentDescriptor segments[gdt_descriptors] = {
    // Null segment
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    // Kernel Code Segment
    {0, 0, 0, SEGMENT_EXECUTE, 1, 0, 1, 0, 0, 1, 0, 0, 0},
    // Kernel Data Segment
    {0, 0, 0, SEGMENT_READ_WRITE, 1, 0, 1, 0, 0, 0, 0, 0, 0},
    // User Code Segment
    {0, 0, 0, SEGMENT_EXECUTE, 1, 3, 1, 0, 0, 1, 0, 0, 0},
    // User Data Segment
    {0, 0, 0, SEGMENT_READ_WRITE, 1, 3, 1, 0, 0, 0, 0, 0, 0},
};

extern "C" [[noreturn]] EFICALL void kernel_main(uint64_t boot_state_address) {
    gdt_pointer.address = reinterpret_cast<uint64_t>(&segments);
    gdt_pointer.limit = sizeof(SegmentDescriptor) * gdt_descriptors;
    asm volatile("lgdt gdt_pointer\n"
                 "mov %%rax, %%rdi\n"// Boot state block
                 "mov $0x10, %%rbx\n"
                 "mov %%rbx, %%ss\n"
                 "mov %%rbx, %%ds\n"
                 "mov %%rbx, %%es\n"
                 "mov %%rbx, %%fs\n"
                 "mov %%rbx, %%gs\n"
                 "pushq $0x08\n"         // Push CS
                 "pushq $kernel_stage2\n"// Push RIP
                 "lretq\n"
                 : /* no output */
                 : "a"(boot_state_address));
    while (true) {}
}
