// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>

class PS2Mouse {
public:
    void interrupt_handler();

private:
    u8 cycle = 0;
    u8 packet[4] = {0};
};
