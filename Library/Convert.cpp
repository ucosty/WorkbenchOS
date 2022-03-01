// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <Convert.h>

namespace Lib {
    static const char character_digits[] = "0123456789ABCDEF";

    void uint8_t_to_cstring(uint8_t n, int base, int buffer_size, char *out) {
        uint8_t output_index = 1;
        uint8_t order_of_magnitude = 1;

        do {
            int digit = n % base;
            out[buffer_size - output_index++] = character_digits[digit];
            n /= base;
            order_of_magnitude *= base;
        } while (n > 0);
    }

    void uint32_t_to_cstring(uint32_t n, int base, int buffer_size, char *out) {
        uint32_t output_index = 1;
        uint32_t order_of_magnitude = 1;

        do {
            int digit = n % base;
            out[buffer_size - output_index++] = character_digits[digit];
            n /= base;
            order_of_magnitude *= base;
        } while (n > 0);
    }

    void uint64_t_to_cstring(uint64_t n, int base, int buffer_size, char *out) {
        uint64_t output_index = 1;
        uint64_t order_of_magnitude = 1;

        do {
            int digit = n % base;
            out[buffer_size - output_index++] = character_digits[digit];
            n /= base;
            order_of_magnitude *= base;
        } while (n > 0);
    }

    void int32_t_to_cstring(int32_t n, int base, int buffer_size, char *out) {
        int is_negative = n < 0;
        if (is_negative) n = -n;

        int output_index = 1;
        int order_of_magnitude = 1;

        do {
            int digit = n % base;
            out[buffer_size - output_index++] = character_digits[digit];
            n /= base;
            order_of_magnitude *= base;
        } while (n > 0);

        if (is_negative && base == 10)
            out[buffer_size - output_index] = '-';
    }

}// namespace Lib