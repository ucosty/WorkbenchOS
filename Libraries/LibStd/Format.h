// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/String.h>
#include <LibStd/StringView.h>
#include <LibStd/Types.h>

extern "C" void putchar(char);

namespace Std {

struct Formatter {
    const char *fmt;
    size_t index;
};

void _println_internal(Formatter &formatter);

template<class T, class... Args>
void _println_internal(Formatter &formatter, T a, Args... args) {
    select(formatter, a);
    formatter.index += 2;
    _println_internal(formatter);
    _println_internal(formatter, args...);
}

template<class... Args>
void println(const char *fmt, Args... args) {
    auto formatter = Formatter{
        .fmt = fmt,
        .index = 0,
    };

    _println_internal(formatter);
    _println_internal(formatter, args...);
    putchar('\n');
}

void select(Formatter &formatter, uint64_t n);
void select(Formatter &formatter, Std::String &string);
void select(Formatter &formatter, Std::String *string);
void select(Formatter &formatter, const Std::String *string);
void select(Formatter &formatter, Std::StringView sv);
void select(Formatter &formatter, const char *string);
void select(Formatter &formatter, void *string);

}// namespace Std
