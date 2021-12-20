// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by matthew on 20/12/2021.
//

#include <cstring.h>

extern "C" void *memset(char *buffer, int value, size_t count) {
    for (size_t i = 0; i < count; i++)
        buffer[i] = (char) value;
    return buffer;
}

extern "C" void *memcpy(void *destination, const void *source, size_t num) {
    char *dst = (char *) destination;
    char *src = (char *) source;
    for (size_t i = 0; i < num; i++) {
        dst[i] = src[i];
    }
    return destination;
}