// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Console.h"
#include "ConsoleFont.h"
#include <LibStd/CString.h>

void Console::initialise(u64 framebuffer, u64 framebuffer_width, u64 framebuffer_height) {
    m_framebuffer = reinterpret_cast<u32 *>(framebuffer);
    m_framebuffer_width = framebuffer_width;
    m_framebuffer_height = framebuffer_height;

    m_double_buffer = new u32[framebuffer_width * framebuffer_height];
    memset(reinterpret_cast<char *>(m_double_buffer), 0x77, m_framebuffer_height * m_framebuffer_width * 4);
    m_framebuffer_size = m_framebuffer_width * m_framebuffer_height * 4;
}


void Console::write_character(char c) {
    if (c == '\n') {
        m_cursor_y += 16;
        m_cursor_x = 0;
        if (m_cursor_y + 16 > m_framebuffer_height) {
            m_cursor_y = m_framebuffer_height - 16;
            scroll(16);
        }
        return;
    }

    auto glyph = font_basic[c];

    if (m_cursor_x + glyph.width > m_framebuffer_width) {
        m_cursor_x = 0;
        m_cursor_y += 16;
    }

    if (m_cursor_y + 16 > m_framebuffer_height) {
        m_cursor_y = m_framebuffer_height - 16;
        scroll(16);
    }

    for (int y = 0; y < 16; y++) {
        auto row = glyph.row[y];
        for (int x = 0; x < glyph.width; x++) {
            if (row & (1 << x)) {
                put_pixel(m_cursor_x + x, m_cursor_y + y, 0);
            }
        }
    }

    m_cursor_x += glyph.width;
}

inline void Console::put_pixel(int x, int y, u32 colour) {
    if (m_double_buffer == nullptr) return;

    auto offset = x + (m_framebuffer_width * y);
    m_double_buffer[offset] = colour;
}

void Console::println(Std::StringView message) {
    for (int i = 0; i < message.length(); i++) {
        write_character(message.get(i));
    }
    m_cursor_y += 16;
    m_cursor_x = 0;
    flip_buffer_screen();
}

void Console::println(const char *message) {
    while (*message != '\0') {
        write_character(*message);
        message++;
    }
    m_cursor_y += 16;
    m_cursor_x = 0;
    flip_buffer_screen();
}

void Console::print(const char *message) {
    while (*message != '\0') {
        write_character(*message);
        message++;
    }
    flip_buffer_screen();
}

void Console::scroll(int lines) {
    auto area_to_scroll = m_framebuffer_width * (m_framebuffer_height - lines);

    auto new_area = lines * m_framebuffer_width;
    auto end_of_framebuffer = m_framebuffer_width * m_framebuffer_height;

    memcpy(m_double_buffer, m_framebuffer + new_area, area_to_scroll * 4);
    memset(reinterpret_cast<char *>(m_double_buffer + end_of_framebuffer - new_area), 0x77, new_area * 4);
}

void Console::flip_buffer_screen() {
    memcpy(m_framebuffer, m_double_buffer, m_framebuffer_size);
}
