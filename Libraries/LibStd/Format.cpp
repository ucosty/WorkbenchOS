// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Format.h"
#include <LibStd/Convert.h>
#include <LibStd/String.h>
#include <LibStd/StringView.h>

namespace Std {
void _println_internal(Formatter &formatter) {
    while (formatter.fmt[formatter.index] != '\0' && formatter.fmt[formatter.index] != '{') {
        putchar(formatter.fmt[formatter.index++]);
    }
}

void select(Formatter &formatter, uint32_t n) {
    char buffer[32] = {0};
    Std::uint32_t_to_cstring(n, 10, 32, buffer);
    for (char c: buffer) {
        if (c == 0) continue;
        putchar(c);
    }
}

void select(Formatter &formatter, uint64_t n) {
    char buffer[32] = {0};
    Std::uint64_t_to_cstring(n, 10, 32, buffer);
    for (char i: buffer) {
        if (i == 0) continue;
        putchar(i);
    }
}

void select(Formatter &formatter, Std::String &string) {
    for (int i = 0; i < string.length(); i++)
        putchar(string.get(i));
}

void select(Formatter &formatter, Std::String *string) {
    for (int i = 0; i < string->length(); i++)
        putchar(string->get(i));
}

void select(Formatter &formatter, const Std::String *string) {
    for (int i = 0; i < string->length(); i++)
        putchar(string->get(i));
}

void select(Formatter &formatter, Std::StringView sv) {
    for (int i = 0; i < sv.length(); i++)
        putchar(sv.get(i));
}

void select(Formatter &formatter, const char *string) {
    for (; *string != '\0'; string++)
        putchar(*string);
}

void select(Formatter &formatter, void *value) {
    char temp[16] = {0};
    Std::uint64_t_to_cstring(reinterpret_cast<uint64_t>(value), 16, 16, temp);
    for (char i: temp) {
        if (i == 0) continue;
        putchar(i);
    }
}
}// namespace Std
