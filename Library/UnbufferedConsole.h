// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

extern "C" void putchar(char c);

struct Formatter {
    const char *fmt;
    size_t index;
};

void select(Formatter *formatter, uint32_t n);
void select(Formatter *formatter, uint64_t n);
void _println_internal(Formatter *formatter);

template <class T>
void _println_internal(Formatter *formatter, T a) {
    select(formatter, a);
    _println_internal(formatter);
}

template <class T, class ...Args>
void _println_internal(Formatter *formatter, T a, Args... args) {
    select(formatter, a);
    formatter->index +=2;
    _println_internal(formatter);
    _println_internal(formatter, args...);
}

template <class ...Args>
void println(const char *fmt, Args... args) {
    auto formatter = Formatter{
        .fmt = fmt,
        .index = 0,
    };

    _println_internal(&formatter);
    _println_internal(&formatter, args...);
    putchar('\r');
    putchar('\n');
}

