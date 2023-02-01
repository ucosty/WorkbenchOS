// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "LibStd/Types.h"

static void inline outl(u16 port, u32 val) {
    asm volatile("outl %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline u32 inl(u16 port) {
    u32 ret;
    asm volatile("inl %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}
