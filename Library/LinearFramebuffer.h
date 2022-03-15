// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

class LinearFramebuffer {
public:
    LinearFramebuffer(uint64_t base, uint64_t width, uint64_t height) : m_width(width), m_height(height), m_framebuffer(reinterpret_cast<uint32_t *>(base)) {}
    void hline(uint64_t x, uint64_t y, uint64_t width, uint32_t colour);
    void vline(uint64_t x, uint64_t y, uint64_t height, uint32_t colour);
    void rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t colour, bool filled);
    uint8_t glyph(uint8_t glyph, uint64_t x, uint64_t y, uint32_t colour);
    void text(const char *text, uint64_t x, uint64_t y, uint32_t colour);

    [[nodiscard]] uint64_t offset_bottom(uint64_t offset) const { return m_height - offset; }
    [[nodiscard]] uint64_t offset_right(uint64_t offset) const { return m_width - offset; }
private:
    uint64_t m_width;
    uint64_t m_height;
    uint32_t *m_framebuffer;
};
