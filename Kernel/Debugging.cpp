// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <UnbufferedConsole.h>
#include <BootConsole/Console.h>
#include <Debugging.h>
#include <LibStd/Error.h>

#include "Platform/x86_64/PortIO.h"

extern Console g_console;

[[noreturn]] void panic() {
    asm volatile("hlt");
    while (true) {}
}

[[noreturn]] void panic(Std::Error error) {
    if(error.is_stringview()) {
        println("PANIC: Got error '{}'\n", error.get_message());
    } else if(error.is_string()) {
//        println("PANIC: Got error {}\n", error.get_string());
    } else {
        println("PANIC: Got error '{}'\n", error.get_code());
    }

    asm volatile("hlt");
    while (true) {}
}

void delay(const size_t microseconds) {
    for (size_t i = 0; i < microseconds; i++)
        PortIO::out8(0x80, 0);
}

void debug_putchar(const char c) {
    PortIO::out8(0xe9, c);
   g_console.write_character(c);
    if(c == '\n') {
        g_console.flip_buffer_screen();
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
