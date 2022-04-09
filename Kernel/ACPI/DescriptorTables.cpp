// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "DescriptorTables.h"
#include <ConsoleIO.h>

using namespace Std;

namespace Kernel {
Result<void> SystemDescriptorTables::initialise(PhysicalAddress rsdp_address) {
    auto rsdp = rsdp_address.as_ptr<RootSystemDescriptionPointer>();
    if (rsdp->revision != 2) {
        printf("PANIC: SystemDescriptorTables revision %d found, expected 2\n", rsdp->revision);
        return Error::from_code(1);
    }

    auto xsdt_address = PhysicalAddress(rsdp->xsdt_address);
    m_xsdt = xsdt_address.as_ptr<SystemDescriptionTableHeader>();
    if (m_xsdt->signature != XSDT_SIGNATURE) {
        printf("PANIC: XSDT signature expected, found %x\n", m_xsdt->signature);
        return Error::from_code(1);
    }

    return {};
}

void SystemDescriptorTables::list_tables() {
    auto entry_count = (m_xsdt->length - sizeof(SystemDescriptionTableHeader)) / sizeof(uint64_t);
    auto *entries = reinterpret_cast<uint64_t *>(m_xsdt + 1);
    for (int i = 0; i < entry_count; i++) {
        auto entry_address = PhysicalAddress(entries[i]);
        auto header = entry_address.as_ptr<SystemDescriptionTableHeader>();
        StringView signature{reinterpret_cast<const char *>(&header->signature), 4};
        printf("Found table: signature = %v, length = %d\n", &signature, header->length);
    }
}
}// namespace Kernel