// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/CString.h>

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

extern "C" size_t strlen(const char *str) {
    size_t count = 0;
    for(; *str != '\0'; str++)
        count++;
    return count;
}
