// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <LibStd/String.h>

void debug_putchar(char c);

struct Formatter {
    const char *fmt;
    size_t index;
};

void select(Formatter *formatter, uint64_t n);
void select(Formatter *formatter, const Std::String &string);
void select(Formatter *formatter, Std::String *string);
void select(Formatter *formatter, const Std::String &string);
void select(Formatter *formatter, Std::StringView sv);
void select(Formatter *formatter, const char *string);
void select(Formatter *formatter, void *string);

void println_internal(Formatter *formatter);

void print_literal_until_field(Formatter * formatter);


// Variadic case: interleave "literal chunk" -> one argument -> repeat.
// IMPORTANT: no `index += 2`; `select()` parses and consumes the whole `{...}`.
template <class T, class... Args>
void println_internal(Formatter* f, T&& a, Args&&... rest) {
    // Print up to next real field (or end)
    print_literal_until_field(f);

    // If no more fields, we're done (ignore extra args silently)
    if (f->fmt[f->index] == '\0') return;

    // At a field start: let select() parse "{...}" and emit the value.
    select(f, a); // consumes up to and including the closing '}'

    // Continue with the remaining arguments
    println_internal(f, rest...);
}

// Driver functions.
// We can keep your shape, but it's cleaner to let the variadic do the first literal too.
template <class... Args>
void print(const char* fmt, Args&&... args) {
    Formatter f{ .fmt = fmt, .index = 0 };
    println_internal(&f, args...);
    println_internal(&f);
}

template <class... Args>
void println(const char* fmt, Args&&... args) {
    print(fmt, args...);
    debug_putchar('\n');
}