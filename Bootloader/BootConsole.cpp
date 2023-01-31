// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "BootConsole.h"
#include "ConsoleFont.h"

void BootConsole::write_character(char c) {
    if(c == '\n') {
        m_cursor_y += 16;
        m_cursor_x = 0;
        return;
    }

    auto glyph = font_basic[c];
    for(int y = 0; y < 16; y++) {
        auto row = glyph.row[y];
        for(int x = 0; x < glyph.width; x++) {
            if(row & (1 << x)) {
                put_pixel(m_cursor_x + x, m_cursor_y + y, 0);
            }
        }
    }

    m_cursor_x += glyph.width;
}

void BootConsole::put_pixel(int x, int y, uint32_t colour) {
    if(m_framebuffer == nullptr) return;

    auto offset = x + (m_framebuffer_width * y);
    m_framebuffer[offset] = colour;
}

void BootConsole::println(const char *message) {
    while (*message != '\0') {
        write_character(*message);
        message++;
    }
    m_cursor_y += 16;
    m_cursor_x = 0;
}

void BootConsole::print(const char *message) {
    while (*message != '\0') {
        write_character(*message);
        message++;
    }
}
