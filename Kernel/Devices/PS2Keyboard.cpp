// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "PS2Keyboard.h"

inline uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0"
                 : "=q"(val)
                 : "Nd"(port));
    return val;
}


void PS2Keyboard::interrupt_handler() {
    auto key = inb(0x60);
    if(m_buffer_items < m_buffer_size - 1) {
        m_buffer[m_buffer_items++] = key;
    }
}

uint8_t PS2Keyboard::read() {
    if(m_buffer_items > 0) {
        return m_buffer[--m_buffer_items];
    }
    return 0;
}
