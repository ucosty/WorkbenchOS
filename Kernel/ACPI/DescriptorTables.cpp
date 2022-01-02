// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "DescriptorTables.h"
#include <ConsoleIO.h>

using namespace ACPI;

Kernel::ACPI g_acpi{};

namespace Kernel {
Result<void> ACPI::initialise(PhysicalAddress rsdp_address) {
    auto rsdp = rsdp_address.as_ptr<RootSystemDescriptionPointer>();
    if (rsdp->revision != 2) {
        printf("PANIC: ACPI revision %d found, expected 2\n", rsdp->revision);
        return Lib::Error::from_code(1);
    }

    auto xsdt_address = PhysicalAddress(rsdp->xsdt_address);
    m_xsdt = xsdt_address.as_ptr<SystemDescriptionTableHeader>();
    if(m_xsdt->signature != XSDT_SIGNATURE) {
        printf("PANIC: XSDT signature expected, found %x\n", m_xsdt->signature);
        return Lib::Error::from_code(1);
    }

    return {};
}
}// namespace Kernel