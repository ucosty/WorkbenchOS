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
    void initialise(uint64_t framebuffer, uint64_t framebuffer_width, uint64_t framebuffer_height);

    void println(Std::StringView message);
    void println(const char *message);
    void print(const char *message);
    void write_character(char c);
    inline void put_pixel(int x, int y, uint32_t colour);

    void scroll(int lines);
    void flip_buffer_screen();

private:
    int m_cursor_x{0};
    int m_cursor_y{0};
    uint32_t *m_framebuffer{nullptr};
    uint32_t *m_double_buffer{nullptr};
    uint64_t m_framebuffer_width{0};
    uint64_t m_framebuffer_height{0};
    size_t m_framebuffer_size{0};
};
