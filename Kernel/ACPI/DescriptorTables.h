// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <ACPI/Tables.h>
#include <PhysicalAddress.h>
#include <Result.h>

namespace Kernel {
class ACPI {
public:
    Result<void> initialise(PhysicalAddress rsdp_address);

private:
    ::ACPI::SystemDescriptionTableHeader *m_xsdt;
};
}

extern Kernel::ACPI g_acpi;