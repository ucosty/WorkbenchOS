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
    void initialise(uint64_t framebuffer, uint64_t framebuffer_width, uint64_t framebuffer_height) {
        m_framebuffer = reinterpret_cast<uint32_t *>(framebuffer);
        m_framebuffer_width = framebuffer_width;
        m_framebuffer_height = framebuffer_height;
    }

    void println(Std::StringView message);
    void println(const char *message);
    void print(const char *message);
    void write_character(char c);
    void put_pixel(int x, int y, uint32_t colour);

private:
    int m_cursor_x{0};
    int m_cursor_y{0};
    uint32_t *m_framebuffer{nullptr};
    uint64_t m_framebuffer_width{0};
    uint64_t m_framebuffer_height{0};
};
