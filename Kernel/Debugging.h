// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by matthew on 23/12/2021.
//

#pragma once

#include <ConsoleIO.h>
#include <Error.h>
#include <Types.h>

#define VERIFY(expression) ({                                             \
    if (!(expression)) {                                                  \
        printf("PANIC: " #expression " failed\n");                        \
        printf("PANIC: %s in %s:%d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
        panic();                                                          \
    }                                                                     \
})

[[noreturn]] void panic();
[[noreturn]] void panic(Lib::Error error);
void delay(size_t microseconds);

namespace Kernel {
const char *memory_map_type(uint8_t memory_type);
}
