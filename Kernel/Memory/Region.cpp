// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Region.h"
#include <ConsoleIO.h>

namespace VirtualMemory {
Region::Region(Region *next, VirtualAddress start, size_t size_in_pages) : m_start(start), m_size_in_pages(size_in_pages), m_next(next) {
}

bool Region::in_region(VirtualAddress address) {
    return address >= m_start && address < address.offset_pages(m_size_in_pages);
}

void MemoryRegion::page_fault_handler() {
    // Map in some memory or error out
}

MemoryRegion::MemoryRegion(Region *next, VirtualAddress start, size_t size_in_pages) : Region(next, start, size_in_pages) {}

void FileRegion::page_fault_handler() {
    // Map in some file data or error out
}

void StackRegion::page_fault_handler() {
    // Grow the stack or error out
}

FreeRegion::FreeRegion(Region *next, VirtualAddress start, size_t size_in_pages) : Region(next, start, size_in_pages) {
}

void FreeRegion::page_fault_handler() {
    // Always a fault
    printf("Unrecoverable page fault detected, aborting!\n");
}
}// namespace VirtualMemory
