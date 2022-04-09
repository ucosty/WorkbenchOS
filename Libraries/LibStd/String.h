// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/StringView.h>
#include <LibStd/Types.h>
#include <LibStd/Vector.h>

namespace Std {
class String {
public:
    String() = default;

    ~String();

    explicit String(const char *characters) : m_characters(characters) {}

    constexpr String(const char *characters, size_t length) : m_characters(characters),
                                                              m_length(length) {}

    String(const String &);

    constexpr explicit String(const StringView &sv) : m_characters(sv.m_characters), m_length(sv.m_length) {}

    [[nodiscard]] size_t length() const { return m_length; }

    [[nodiscard]] const char *as_cstring() const { return const_cast<char *>(m_characters); }

    [[nodiscard]] inline constexpr char get(size_t index) const { return m_characters[index]; }

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
}// namespace Std
