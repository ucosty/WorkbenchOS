// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <LinearFramebuffer.h>
#include <ConsoleFont.h>

void LinearFramebuffer::hline(uint64_t x, uint64_t y, uint64_t width, uint32_t colour) {
    uint32_t position = (y * m_width) + x;
    for(int i = 0; i < width; i++) {
        m_framebuffer[position++] = colour;
    }
}

void LinearFramebuffer::vline(uint64_t x, uint64_t y, uint64_t height, uint32_t colour) {
    uint32_t position = (y * m_height) + x;
    for(int i = 0; i < height; i++) {
        m_framebuffer[position] = colour;
        position += m_width;
    }
}

void LinearFramebuffer::rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour, bool filled) {
    if(filled) {
        for(int i = 0; i < h; i++) {
            hline(x, y + i, w, colour);
        }
    } else {
        hline(x, y, w, colour); // top
        hline(x, y + h, w, colour); // bottom
        vline(x, y, h, colour); // left
        vline(x + w, y, h, colour); // right
    }
}

uint8_t LinearFramebuffer::glyph(uint8_t glyph, uint64_t x, uint64_t y, uint32_t colour) {
    glyph_t g = glyphs[glyph];
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < g.width; j++) {
            if(g.row[i] == 0) {
                continue;
            }
            if(g.row[i] & (1 << j)) {
                auto offset = ((y + i) * m_width) + x + j;
                m_framebuffer[offset] = colour;
            }
        }
    }
    return g.width;
}

void LinearFramebuffer::text(const char *text, uint64_t x, uint64_t y, uint32_t colour) {
    while(*text != '\0') {
        x += glyph((uint8_t) *text++, x, y, colour);
    }
}
