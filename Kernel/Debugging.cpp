// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <Types.h>

void inline outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

void debug_putstring(const char *string) {
    while (*string != '\0') {
        outb(0xe9, *string);
        string++;
    }
}

