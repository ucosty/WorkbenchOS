// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>

class PS2Keyboard {
public:
    void interrupt_handler();

    [[nodiscard]] bool is_buffer_empty() const { return m_buffer_items == 0; }
    uint8_t read();

private:
    size_t m_buffer_items{0};
    static constexpr size_t m_buffer_size = 128;
    uint8_t m_buffer[m_buffer_size] = {0};
};
