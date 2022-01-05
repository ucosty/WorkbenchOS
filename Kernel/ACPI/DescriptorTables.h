// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "DescriptorTables.h"
#include <ACPI/Tables.h>
#include <PhysicalAddress.h>
#include <Result.h>

namespace Kernel {
class SystemDescriptorTables {
public:
    Result<void> initialise(PhysicalAddress rsdp_address);
    Result<::Lib::SystemDescriptionTableHeader *> find_madt();
    void parse_madt(::Lib::SystemDescriptionTableHeader *);

private:
    ::Lib::SystemDescriptionTableHeader *m_xsdt{nullptr};
};
}// namespace Kernel
