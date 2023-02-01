// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "PS2Mouse.h"
#include <UnbufferedConsole.h>

inline u8 inb(u16 port) {
    u8 val;
    asm volatile("inb %1, %0"
                 : "=q"(val)
                 : "Nd"(port));
    return val;
}

void PS2Mouse::interrupt_handler() {
    auto value = inb(0x60);

    switch (cycle) {
        case 0: {
            packet[0] = value;
            cycle++;
            break;
        }
        case 1: {
            packet[1] = value;
            cycle++;
            break;
        }
        case 2: {
            if (packet[0] & 0x80 || packet[0] & 0x40) {
                cycle = 0;
                break;
            }
            packet[2] = value;

            i32 rel_x = packet[1];
            if (packet[0] & 0x10) {
                rel_x = packet[1] | 0xFFFFFF00;
            }

            i32 rel_y = packet[2];
            if (packet[0] & 0x20) {
                rel_y = packet[2] | 0xFFFFFF00;
            }

//            printf("X: %d      Y: %d\n", rel_x, rel_y);

            cycle = 0;
            break;
        }
        default:
            println("Unknown state {}", cycle);
    }
}
