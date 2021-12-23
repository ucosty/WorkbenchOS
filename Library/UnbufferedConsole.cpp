// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <Convert.h>
#include <Types.h>
#include <UnbufferedConsole.h>

void select(Formatter *formatter, uint32_t n) {
    debug_putchar('#');
    char buffer[32] = {0};
    Lib::uint32_t_to_cstring(n, 10, 32, buffer);
    for (char c: buffer) {
        if (c == 0) continue;
        debug_putchar(c);
    }
}

void select(Formatter *formatter, uint64_t n) {
    debug_putchar('!');
    char buffer[32] = {0};
    Lib::uint64_t_to_cstring(n, 10, 32, buffer);
    for (char i: buffer) {
        if (i == 0) continue;
        debug_putchar(i);
    }
}

void _println_internal(Formatter *formatter) {
    while (formatter->fmt[formatter->index] != '\0' && formatter->fmt[formatter->index] != '{') {
        debug_putchar(formatter->fmt[formatter->index++]);
    }
}

void println(const char *fmt) {
    auto formatter = Formatter{
        .fmt = fmt,
        .index = 0,
    };

    _println_internal(&formatter);
    debug_putchar('\r');
    debug_putchar('\n');
}
