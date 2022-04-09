// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "DescriptorTables.h"
#include <ACPI/Tables.h>
#include <ConsoleIO.h>
#include <PhysicalAddress.h>
#include "LibStd/Result.h"
#include "LibStd/StringView.h"

using namespace Std;

namespace Kernel {

template<typename T>
class DescriptorTable {
public:
    DescriptorTable(::Std::SystemDescriptionTableHeader *_header, T *_table)
        : header(_header), table(_table) {}
    ::Std::SystemDescriptionTableHeader *header;
    T *table;
};

class SystemDescriptorTables {
public:
    Std::Result<void> initialise(PhysicalAddress rsdp_address);

    template<typename T>
    Std::Result<DescriptorTable<T>> find_table(const Std::StringView signature_to_find) {
        auto entry_count = (m_xsdt->length - sizeof(SystemDescriptionTableHeader)) / sizeof(uint64_t);
        auto *entries = reinterpret_cast<uint64_t *>(m_xsdt + 1);
        for (int i = 0; i < entry_count; i++) {
            auto entry_address = PhysicalAddress(entries[i]);
            auto header = entry_address.as_ptr<SystemDescriptionTableHeader>();
            auto table = reinterpret_cast<T *>(header + 1);
            Std::StringView signature{reinterpret_cast<const char *>(&header->signature), 4};
            if (signature == signature_to_find) {
                return DescriptorTable<T>(header, table);
            }
        }
        return Std::Error::from_code(1);
    }

    void list_tables();

private:
    ::Std::SystemDescriptionTableHeader *m_xsdt{nullptr};
};
}// namespace Kernel
