// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by matthew on 23/12/2021.
//

#pragma once

#include <Error.h>
#include <Types.h>

#define VERIFY(expression) ({      \
    if (!(expression))             \
        panic(Lib::Error::from_code(1)); \
})

[[noreturn]] void panic(Lib::Error error);
void delay(size_t microseconds);

namespace Kernel {
const char *memory_map_type(uint8_t memory_type);
}
