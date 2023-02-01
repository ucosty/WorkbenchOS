// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>
#include <LibStd/StringView.h>
#include <LibStd/String.h>

class Console {
public:
    void initialise(u64 framebuffer, u64 framebuffer_width, u64 framebuffer_height);

    void println(Std::StringView message);
    void println(const char *message);
    void print(const char *message);
    void write_character(char c);
    inline void put_pixel(int x, int y, u32 colour);

    void scroll(int lines);
    void flip_buffer_screen();

private:
    int m_cursor_x{0};
    int m_cursor_y{0};
    u32 *m_framebuffer{nullptr};
    u32 *m_double_buffer{nullptr};
    u64 m_framebuffer_width{0};
    u64 m_framebuffer_height{0};
    size_t m_framebuffer_size{0};
};
