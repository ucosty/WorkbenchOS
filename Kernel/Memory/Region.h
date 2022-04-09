// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <LibStd/String.h>
#include <VirtualAddress.h>
#include "LibStd/Result.h"

using namespace Std;

namespace VirtualMemory {
enum class RegionType {
    File,
    Memory,
    Stack,
    Free
};

class Region {
public:
    explicit Region(Region *next, VirtualAddress start, size_t size_in_pages);
    bool in_region(VirtualAddress address);



    virtual void page_fault_handler() = 0;

    [[nodiscard]] Region *next() const { return m_next; }

protected:
    String m_name;
    VirtualAddress m_start;
    size_t m_size_in_pages;
    Region *m_next;
};

class FreeRegion : public Region {
public:
    explicit FreeRegion(Region *next, VirtualAddress start, size_t size_in_pages);
    void page_fault_handler() override;
};

class MemoryRegion : public Region {
public:
    explicit MemoryRegion(Region *next, VirtualAddress start, size_t size_in_pages);
    void page_fault_handler() override;
};

class FileRegion : public Region {
public:
    void page_fault_handler() override;
};

class StackRegion : public Region {
public:
    void page_fault_handler() override;
};
}// namespace VirtualMemory
