// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "ACPI.h"
#include "../APIC.h"
#include "../Debugging.h"
#include "../Memory/MemoryManager.h"
#include "../Processor.h"
#include <ConsoleIO.h>
#include <LibStd/CString.h>
#include <LibStd/Try.h>

using namespace Std;

static volatile bool g_boot_spinlock = false;
extern "C" void ap_main();
extern "C" void *ap_trampoline;
extern "C" void *end_ap_trampoline;

Result<void> Kernel::ACPI::initialise(PhysicalAddress rsdp_address) {
    TRY(m_descriptor_tables.initialise(rsdp_address));
    return {};
}

void Kernel::ACPI::start_application_processors() {
    auto &memory_manager = Kernel::MemoryManager::get_instance();

    // Set up the trampoline function
    // This function is run by the processor and transitions it from real mode to long mode
    // before jumping into the kernel ap_main()
    auto trampoline = PhysicalAddress(0x8000);
    size_t ap_init_size = reinterpret_cast<size_t>(&end_ap_trampoline) - reinterpret_cast<size_t>(&ap_trampoline);
    memcpy(trampoline.as_ptr(), &ap_trampoline, ap_init_size);

    // Set up variables for the trampoline
    uint64_t cr3 = 0;
    asm volatile("mov %%cr3, %%rax"
                 : "=a"(cr3));

    auto ap_cr3 = PhysicalAddress(0x8ff0).as_ptr<uint64_t>();
    *ap_cr3 = cr3;

    auto ap_main_function = PhysicalAddress(0x8fe0).as_ptr<uint64_t>();
    *ap_main_function = reinterpret_cast<uint64_t>(&ap_main);

    uint64_t stack_base = 0x9000;
    auto stack = PhysicalAddress(0x8fd0).as_ptr<uint64_t>();

    auto cpus = 4;

    // Boot the processors
    for (int i = 1; i < cpus; i++) {
        *stack = stack_base + (i * Page);
        g_boot_spinlock = true;
        start_application_processor(i);
        while (g_boot_spinlock) {}
    }

    memory_manager.unmap_identity_mapping();
}

Result<void> Kernel::ACPI::find_devices() {
    auto fadt = TRY(m_descriptor_tables.find_table<FixedAcpiDescriptionTable>("FACP"_sv));
    auto dsdt_address = PhysicalAddress(fadt.table->x_dsdt).as_mapped_address();
    println("fadt: revision = {}, length = {}, x_dsdt = {}", fadt.header->revision, fadt.header->length, dsdt_address);

    auto *dsdt_header = reinterpret_cast<SystemDescriptionTableHeader *>(dsdt_address);
    println("dsdt_address: revision = {}, length = {}", dsdt_header->revision, dsdt_header->length - sizeof(SystemDescriptionTableHeader));

    auto mcfg_or_error = m_descriptor_tables.find_table<MCFGTable>("MCFG"_sv);
    if (!mcfg_or_error.is_error()) {
        auto mcfg = mcfg_or_error.get();
        println("mcfg_address: revision = {}, length = {}", mcfg.header->revision, mcfg.header->length - sizeof(MCFGTable));
    } else {
        println("MCFG table not found");
    }

    m_descriptor_tables.list_tables();

    return {};
}

void Kernel::ACPI::start_application_processor(int id) {
    APIC::send_init(id);
    delay(10 * 1000);
    APIC::send_sipi(id);
}

void Kernel::ACPI::set_booted() {
    g_boot_spinlock = false;
}
