// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <ConsoleIO.h>
#include <Debugging.h>
#include <LibStd/Error.h>

[[noreturn]] void panic() {
    asm volatile("hlt");
    while (true) {}
}

[[noreturn]] void panic(Std::Error error) {
    printf("\u001b[31mPANIC:\u001b[0m Got error %d\n", error);
    asm volatile("hlt");
    while (true) {}
}

void inline outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

void delay(size_t microseconds) {
    for (size_t i = 0; i < microseconds; i++)
        outb(0x80, 0);
}

void debug_putchar(char c) {
    outb(0xe9, c);
}

void debug_putstring(const char *string) {
    while (*string != '\0') {
        outb(0xe9, *string);
        string++;
    }
}

namespace Kernel {
const char *memory_map_type(uint8_t memory_type) {
    switch (memory_type) {
        case 0:
            return "EFI_RESERVED_MEMORY_TYPE";
        case 1:
            return "EFI_LOADER_CODE";
        case 2:
            return "EFI_LOADER_DATA";
        case 3:
            return "EFI_BOOT_SERVICES_CODE";
        case 4:
            return "EFI_BOOT_SERVICES_DATA";
        case 5:
            return "EFI_RUNTIME_SERVICES_CODE";
        case 6:
            return "EFI_RUNTIME_SERVICES_DATA";
        case 7:
            return "EFI_CONVENTIONAL_MEMORY";
        case 8:
            return "EFI_UNUSABLE_MEMORY";
        case 9:
            return "EFI_ACPI_RECLAIM_MEMORY";
        case 10:
            return "EFI_ACPI_MEMORY_NVS";
        case 11:
            return "EFI_MEMORY_MAPPED_IO";
        case 12:
            return "EFI_MEMORY_MAPPED_IO_PORT_SPACE";
        case 13:
            return "EFI_PAL_CODE";
        case 14:
            return "EFI_PERSISTENT_MEMORY";
        default:
            return "Unknown";
    }
}
}// namespace Kernel
