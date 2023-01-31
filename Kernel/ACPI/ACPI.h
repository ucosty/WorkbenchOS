// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <ACPI/DescriptorTables.h>
#include <LibStd/Result.h>

namespace Kernel {
class ACPI {
public:
    static ACPI &get_instance() {
        static ACPI instance;
        return instance;
    }
    ACPI(ACPI const &) = delete;
    void operator=(ACPI const &) = delete;

    Std::Result<void> initialise(PhysicalAddress rsdp_address);

    // Device Discovery
    [[nodiscard]] bool has_mcfg_table() const { return false; }
    Std::Result<void> find_devices();

    template<typename T>
    Std::Result<DescriptorTable< T>> find_table(const Std::StringView signature_to_find) {
        return m_descriptor_tables.find_table<T>(signature_to_find);
    }

    // Multi-Processor Initialisation
    void start_application_processors();
    static void set_booted();

private:
    ACPI() = default;
    SystemDescriptorTables m_descriptor_tables{};
    static void start_application_processor(int id);
};
}// namespace Kernel
