// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Error.h>
#include <LibStd/Types.h>
#include <UnbufferedConsole.h>

#define VERIFY(expression) ({                                                   \
    if (!(expression)) {                                                        \
        println("PANIC: " #expression " failed");                               \
        println("PANIC: {} in {}:{}", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
        panic();                                                                \
    }                                                                           \
})

[[noreturn]] void panic();
[[noreturn]] void panic(Std::Error error);
void delay(size_t microseconds);
