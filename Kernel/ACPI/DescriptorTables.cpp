// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "DescriptorTables.h"
#include <ConsoleIO.h>

using namespace Lib;

namespace Kernel {
Result<void> SystemDescriptorTables::initialise(PhysicalAddress rsdp_address) {
    auto rsdp = rsdp_address.as_ptr<RootSystemDescriptionPointer>();
    if (rsdp->revision != 2) {
        printf("PANIC: SystemDescriptorTables revision %d found, expected 2\n", rsdp->revision);
        return Lib::Error::from_code(1);
    }

    auto xsdt_address = PhysicalAddress(rsdp->xsdt_address);
    m_xsdt = xsdt_address.as_ptr<SystemDescriptionTableHeader>();
    if (m_xsdt->signature != XSDT_SIGNATURE) {
        printf("PANIC: XSDT signature expected, found %x\n", m_xsdt->signature);
        return Lib::Error::from_code(1);
    }

    return {};
}

Result<SystemDescriptionTableHeader *> SystemDescriptorTables::find_madt() {
    auto entry_count = (m_xsdt->length - sizeof(SystemDescriptionTableHeader)) / sizeof(uint64_t);
    auto *entries = reinterpret_cast<uint64_t *>(m_xsdt + 1);
    for (int i = 0; i < entry_count; i++) {
        auto entry_address = PhysicalAddress(entries[i]);
        auto entry = entry_address.as_ptr<SystemDescriptionTableHeader>();
        if (entry->signature == MADT_SIGNATURE) {
            return entry;
        }
    }

    printf("PANIC: Could not find MADT entry in APIC XSDT\n");
    return Lib::Error::from_code(1);
}

static const char *madt_type(uint8_t type) {
    switch (type) {
        case 0:
            return "Local APIC";
        case 1:
            return "I/O APIC";
        case 2:
            return "I/O APIC Interrupt Source Override";
        case 3:
            return "I/O APIC Non-maskable interrupt source";
        case 4:
            return "Local APIC Non-maskable interrupts";
        case 5:
            return "Local APIC Address Override";
        case 9:
            return "Processor Local x2APIC";
        default:
            return "Invalid";
    }
}

void SystemDescriptorTables::parse_madt(SystemDescriptionTableHeader *table_header) {
    if (table_header->signature != MADT_SIGNATURE)
        return;

    // Get the MADT header
    auto next_entry_address = reinterpret_cast<uint64_t>(table_header + 1);
    auto madt_header = reinterpret_cast<MultipleApicDescriptionTable *>(next_entry_address);
    next_entry_address += sizeof(MultipleApicDescriptionTable);

    // Get the MADT entries
    auto madt_end_address = next_entry_address + table_header->length - sizeof(MultipleApicDescriptionTable) - sizeof(SystemDescriptionTableHeader);
    auto i = 0;
    while (madt_end_address - next_entry_address > 0) {
        auto entry = reinterpret_cast<MultipleApicDescriptionEntry *>(next_entry_address);
//        printf("%d: type = %s, length = %d\n", i, madt_type(entry->type), entry->length);
        next_entry_address += entry->length;
        switch (entry->type) {
            case 0: {
                auto local_apic = reinterpret_cast<MadtLocalApicEntry *>(entry);

                printf("%d: type = CPU, id = %d, online = %d, can_enable = %d\n", i, local_apic->processor_id, (local_apic->flags >> 1) & 1, local_apic->flags & 1);
                break;
            }
        }

        i++;
    }
}


}// namespace Kernel