// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <Types.h>

namespace Lib {
    void uint8_t_to_cstring(uint8_t n, int base, int buffer_size, char *out);
    void int32_t_to_cstring(int32_t n, int base, int buffer_size, char *out);
    void uint32_t_to_cstring(uint32_t n, int base, int buffer_size, char *out);
    void uint64_t_to_cstring(uint64_t n, int base, int buffer_size, char *out);
}
