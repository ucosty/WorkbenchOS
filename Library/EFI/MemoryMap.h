// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <LibStd/Iterator.h>
#include <LibStd/Types.h>

namespace EFI {
enum MemoryType {
    EFI_RESERVED_MEMORY_TYPE,
    EFI_LOADER_CODE,
    EFI_LOADER_DATA,
    EFI_BOOT_SERVICES_CODE,
    EFI_BOOT_SERVICES_DATA,
    EFI_RUNTIME_SERVICES_CODE,
    EFI_RUNTIME_SERVICES_DATA,
    EFI_CONVENTIONAL_MEMORY,
    EFI_UNUSABLE_MEMORY,
    EFI_ACPI_RECLAIM_MEMORY,
    EFI_ACPI_MEMORY_NVS,
    EFI_MEMORY_MAPPED_IO,
    EFI_MEMORY_MAPPED_IO_PORT_SPACE,
    EFI_PAL_CODE,
    EFI_PERSISTENT_MEMORY
};

struct MemoryDescriptor {
    uint32_t type;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t number_of_pages;
    uint64_t attribute;
    uint64_t pad;
};

struct PACKED MemoryMap {
    MemoryDescriptor *m_descriptors{nullptr};
    uint64_t m_size{0};
    uint64_t m_descriptor_count{0};
    uint64_t m_key{0};
    uint64_t m_descriptor_size{0};
    uint32_t m_descriptor_version{0};

    [[nodiscard]] Std::Iterator<MemoryDescriptor> begin() const { return {&m_descriptors[0]}; }
    [[nodiscard]] Std::Iterator<MemoryDescriptor> end() const { return {&m_descriptors[m_descriptor_count]}; }
};
}// namespace EFI