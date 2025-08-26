// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/CString.h>

extern "C" int memcmp(const void *s1, const void *s2, size_t n) {
    const auto *b1 = static_cast<const u8 *>(s1);
    const auto *b2 = static_cast<const u8 *>(s2);
    for (size_t i = 0; i < n; i++) {
        if (b1[i] != b2[i]) {
            return 1;
        }
    }
    return 0;
}


extern "C" void *memset(char *buffer, int value, size_t count) {
    for (size_t i = 0; i < count; i++)
        buffer[i] = (char) value;
    return buffer;
}

extern "C" void *memcpy(void *destination, const void *source, size_t num) {
    if (destination == nullptr || source == nullptr) return nullptr;
    auto source_address = reinterpret_cast<size_t>(source);
    auto destination_address = reinterpret_cast<size_t>(destination);

    // Perfect alignment
    if (source_address % 8 == 0 && destination_address % 8 == 0 && num % 8 == 0) {
        auto *src = reinterpret_cast<const uint64_t *>(source);
        auto *dst = reinterpret_cast<uint64_t *>(destination);
        auto quad_words = num / sizeof(uint64_t);

        for (int i = 0; i < quad_words; i++) {
            dst[i] = src[i];
        }

        return destination;
    }

    if (source_address % 4 == 0 && destination_address % 4 == 0 && num % 4 == 0) {
        auto *src = reinterpret_cast<const uint32_t *>(source);
        auto *dst = reinterpret_cast<uint32_t *>(destination);
        auto quad_words = num / sizeof(uint32_t);

        for (int i = 0; i < quad_words; i++) {
            dst[i] = src[i];
        }

        return destination;
    }

    // Not perfect alignment
    char *dst = (char *) destination;
    char *src = (char *) source;
    for (size_t i = 0; i < num; i++) {
        dst[i] = src[i];
    }
    return destination;
}

extern "C" void *memmove(void *destination, const void *source, size_t num) {
    auto source_address = reinterpret_cast<size_t>(source);
    auto destination_address = reinterpret_cast<size_t>(destination);

    // Perfect alignment
    if (source_address % 8 == 0 && destination_address % 8 == 0 && num % 8 == 0) {
        auto *src = reinterpret_cast<const uint64_t *>(source);
        auto *dst = reinterpret_cast<uint64_t *>(destination);
        auto quad_words = num / 8;

        for (int i = 0; i < quad_words; i++) {
            dst[i] = src[i];
        }
    }

    return nullptr;
}

extern "C" size_t strlen(const char *str) {
    size_t count = 0;
    for (; *str != '\0'; str++)
        count++;
    return count;
}

static uint32_t source_table[32] = {0};
static uint32_t destination_table[32] = {0};

[[noreturn]] void loop_de_loop() {
    int i = 0;
    while (true) {
        source_table[i]++;
        memcpy(destination_table, source_table, 32 * sizeof(uint32_t));
    }
}