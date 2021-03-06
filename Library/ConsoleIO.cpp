// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <ConsoleIO.h>
#include <EFI/Efi.h>
#include <LibStd/Convert.h>
#include <LibStd/String.h>
#include <LibStd/StringView.h>
#include <LibStd/Types.h>
#include <LibStd/Variadic.h>

using namespace Std;

void printf(const char *fmt...) {
    va_list args;
    va_start(args, fmt);

    // TODO: Fixed buffer, let's allocate one later
    int output_index = 0;
    char buffer[200];
    for (char &i: buffer)
        i = 0;

    bool is_formatting = false;
    while (*fmt != '\0') {
        if (!is_formatting && *fmt == '%') {
            is_formatting = true;
            fmt++;
            continue;
        }

        // Handle string
        if (is_formatting) {
            switch (*fmt) {
                case 'c': {
                    char character = va_arg(args, int);
                    buffer[output_index++] = character;
                    break;
                }
                case 's': {
                    char *str = va_arg(args, char *);
                    while (*str != '\0') {
                        buffer[output_index++] = *str;
                        str++;
                    }
                    break;
                }
                case 'S': {
                    wchar_t *str = va_arg(args, wchar_t *);
                    while (*str != '\0') {
                        buffer[output_index++] = *str;
                        str++;
                    }
                    break;
                }
                case 'd':
                case 'i': {
                    // Max length value "-2147483648" ~11 chars
                    char temp[12] = {0};
                    int d = va_arg(args, int);
                    int32_t_to_cstring(d, 10, 12, temp);
                    for (char i: temp) {
                        if (i == 0) continue;
                        buffer[output_index++] = i;
                    }
                    break;
                }
                case 'v': {
                    // StringView
                    auto string_view = va_arg(args, StringView *);
                    for (size_t i = 0; i < string_view->length(); i++) {
                        buffer[output_index++] = string_view->get(i);
                    }
                    break;
                }
                case 'V': {
                    // String
                    auto string = va_arg(args, String *);
                    for (size_t i = 0; i < string->length(); i++) {
                        buffer[output_index++] = string->get(i);
                    }
                    break;
                }
                case 'x': {
                    // Max length value "FFFFFFFF" ~8 characters
                    char temp[8] = {0};
                    uint32_t d = va_arg(args, uint32_t);
                    uint32_t_to_cstring(d, 16, 8, temp);
                    for (char i: temp) {
                        if (i == 0) continue;
                        buffer[output_index++] = i;
                    }
                    break;
                }
                case 'X': {
                    // Max length value "FFFFFFFFFFFFFFFF" ~16 characters
                    char temp[16] = {0};
                    uint64_t d = va_arg(args, uint64_t);
                    uint64_t_to_cstring(d, 16, 16, temp);
                    for (char i: temp) {
                        if (i == 0) continue;
                        buffer[output_index++] = i;
                    }
                    break;
                }
            }

            is_formatting = false;
            fmt++;
            continue;
        }

        buffer[output_index++] = *fmt;
        fmt++;
    }
    debug_putstring(buffer);
    va_end(args);
}
