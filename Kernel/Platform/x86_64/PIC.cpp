// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

namespace PIC {
void disable() {
    asm volatile("mov $0xff, %al\n"
                 "outb %al, $0xa1\n"
                 "outb %al, $0x21\n");
}
}// namespace PIC
