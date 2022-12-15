// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <ConsoleIO.h>
#include <Debugging.h>
#include <LibStd/Error.h>
#include <BootConsole/Console.h>

extern Console g_console;

[[noreturn]] void panic() {
    asm volatile("hlt");
    while (true) {}
}

[[noreturn]] void panic(Std::Error error) {
//    printf("\u001b[31mPANIC:\u001b[0m Got error %d\n", error);
    printf("PANIC: Got error %d\n", error);
//    g_console.println("PANIC: Got error %d", error.get_code());
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
    g_console.print(string);
    while (*string != '\0') {
        outb(0xe9, *string);
        string++;
    }
}

namespace Kernel {
const char *memory_map_type[] = {
    "EFI_RESERVED_MEMORY_TYPE",
    "EFI_LOADER_CODE",
    "EFI_LOADER_DATA",
    "EFI_BOOT_SERVICES_CODE",
    "EFI_BOOT_SERVICES_DATA",
    "EFI_RUNTIME_SERVICES_CODE",
    "EFI_RUNTIME_SERVICES_DATA",
    "EFI_CONVENTIONAL_MEMORY",
    "EFI_UNUSABLE_MEMORY",
    "EFI_ACPI_RECLAIM_MEMORY",
    "EFI_ACPI_MEMORY_NVS",
    "EFI_MEMORY_MAPPED_IO",
    "EFI_MEMORY_MAPPED_IO_PORT_SPACE",
    "EFI_PAL_CODE",
    "EFI_PERSISTENT_MEMORY",
};
}// namespace Kernel
