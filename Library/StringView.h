// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

namespace Lib {
class String;

template<typename T>
class Vector;

class StringView {
public:
    StringView() {}

    explicit StringView(const char *characters) : m_characters(characters) {}

    constexpr StringView(const char *characters, size_t length) : m_characters(characters),
                                                                  m_length(length) {}

    [[nodiscard]] inline constexpr const char *as_cstring() const { return m_characters; }

    [[nodiscard]] inline constexpr size_t length() const { return m_length; }

    [[nodiscard]] inline constexpr char get(size_t index) const { return m_characters[index]; }

    [[nodiscard]] inline constexpr int find_first(char c) const {
        for (int i = 0; i < m_length; i++) {
            if (m_characters[i] == c)
                return i;
        }
        return -1;
    }

    [[nodiscard]] inline constexpr StringView substring(const size_t offset, const size_t length = -1) const {
        auto substring_length = length == -1 ? m_length - offset : length;
        if (substring_length > m_length) {
            substring_length = m_length;
        }
        return {&m_characters[offset], substring_length};
    }

    [[nodiscard]] inline constexpr bool equals(const StringView &other) {
        if (m_length != other.m_length)
            return false;

        for (size_t i = 0; i < m_length; i++) {
            if (m_characters[i] != other.m_characters[i])
                return false;
        }

        return true;
    }

    friend bool operator== (const StringView& a, const StringView& b) {
        if(a.m_length != b.m_length)
            return false;

        for(size_t i = 0; i < a.m_length; i++) {
            if(a.m_characters[i] != b.m_characters[i])
                return false;
        }

        return true;
    }

    Vector<StringView> split(char);

private:
    friend class String;
    const char *m_characters{nullptr};
    size_t m_length;
};
}// namespace Lib

constexpr Lib::StringView operator"" _sv(const char *characters, size_t length) {
    return {characters, length};
}
