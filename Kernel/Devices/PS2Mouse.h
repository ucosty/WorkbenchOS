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
    uint8_t cycle = 0;
    uint8_t packet[4] = {0};
};
