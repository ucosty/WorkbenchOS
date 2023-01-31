// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/StringView.h>
#include <LibStd/Types.h>

namespace Std {

class String {
public:
    String() = default;

    ~String();

    String(const char *characters);

    String(const char *characters, size_t length);

    String(const String &other);

    explicit String(const StringView &sv);

    [[nodiscard]] size_t length() const { return m_length; }

    [[nodiscard]] const char *as_cstring() const { return const_cast<char *>(m_characters); }

    [[nodiscard]] inline constexpr char get(size_t index) const { return m_characters[index]; }

    char &operator[](size_t index) const {
        return const_cast<char &>(m_characters[index]);
    }

private:
    const char *m_characters{nullptr};
    size_t m_length{0};
};

inline bool operator==(const String &lhs, const StringView &rhs) {
    if (lhs.length() != rhs.length()) return false;
    for (int i = 0; i < lhs.length(); i++) {
        if (lhs.get(i) != rhs.get(i)) return false;
    }
    return true;
}

inline bool operator==(const String &lhs, const String &rhs) {
    if (lhs.length() != rhs.length()) return false;
    for (int i = 0; i < lhs.length(); i++) {
        if (lhs.get(i) != rhs.get(i)) return false;
    }
    return true;
}
}// namespace Std
