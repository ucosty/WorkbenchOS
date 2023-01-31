// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

//#include <LibStd/StringView.h>
#include <LibStd/Types.h>

class BootConsole {
public:
    void initialise(uint32_t *framebuffer, int framebuffer_width, int framebuffer_height) {
        m_framebuffer = framebuffer;
        m_framebuffer_width = framebuffer_width;
        m_framebuffer_height = framebuffer_height;
    }

//    void println(Std::StringView message);
    void println(const char *message);
    void print(const char *message);
    void write_character(char c);
    void put_pixel(int x, int y, uint32_t colour);

private:
    int m_cursor_x{0};
    int m_cursor_y{0};
    uint32_t *m_framebuffer{nullptr};
    int m_framebuffer_width{0};
    int m_framebuffer_height{0};
};
