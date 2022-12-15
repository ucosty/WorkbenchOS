// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Console.h"
#include "ConsoleFont.h"

void Console::write_character(char c) {
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

void Console::put_pixel(int x, int y, uint32_t colour) {
    if(m_framebuffer == nullptr) return;

    auto offset = x + (m_framebuffer_width * y);
    m_framebuffer[offset] = colour;
}

void Console::println(Std::StringView message) {
    for(int i = 0; i < message.length(); i++) {
        write_character(message.get(i));
    }
    m_cursor_y += 16;
    m_cursor_x = 0;
}

void Console::println(const char *message) {
    while (*message != '\0') {
        write_character(*message);
        message++;
    }
    m_cursor_y += 16;
    m_cursor_x = 0;
}

void Console::print(const char *message) {
    while (*message != '\0') {
        write_character(*message);
        message++;
    }
}
