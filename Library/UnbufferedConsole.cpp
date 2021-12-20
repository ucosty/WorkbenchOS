// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <Convert.h>
#include <Types.h>
#include <UnbufferedConsole.h>

void select(Formatter *formatter, uint32_t n) {
    putchar('#');
    char buffer[32] = {0};
    Lib::uint32_t_to_cstring(n, 10, 32, buffer);
    for (char c: buffer) {
        if (c == 0) continue;
        putchar(c);
    }
}

void select(Formatter *formatter, uint64_t n) {
    putchar('!');
    char buffer[32] = {0};
    Lib::uint64_t_to_cstring(n, 10, 32, buffer);
    for (char i: buffer) {
        if (i == 0) continue;
        putchar(i);
    }
}

void _println_internal(Formatter *formatter) {
    while (formatter->fmt[formatter->index] != '\0' && formatter->fmt[formatter->index] != '{') {
        putchar(formatter->fmt[formatter->index++]);
    }
}

void println(const char *fmt) {
    auto formatter = Formatter{
        .fmt = fmt,
        .index = 0,
    };

    _println_internal(&formatter);
    putchar('\r');
    putchar('\n');
}
