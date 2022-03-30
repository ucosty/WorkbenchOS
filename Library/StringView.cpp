// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <ConsoleIO.h>
#include <StringView.h>
#include <Vector.h>

namespace Lib {
Vector<StringView> StringView::split(char c) {
    Vector<StringView> result;
    auto substring_start = 0;
    for (auto i = 0; i < m_length; i++) {
        if (m_characters[i] == c) {
            auto length = i - substring_start;
            result.append(StringView(&m_characters[substring_start], length));
            substring_start = i + 1;
        }
    }

    auto length = m_length - substring_start;
    result.append(StringView(&m_characters[substring_start], length));

    return result;
}
}// namespace Lib
