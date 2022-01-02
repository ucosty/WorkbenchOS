// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <StringView.h>
#include <Types.h>

namespace Lib {
class String {
public:
    String() = default;

    explicit String(const char *characters) : m_characters(characters) {}

    constexpr String(const char *characters, size_t length) : m_characters(characters),
                                                              m_length(length) {}

    constexpr explicit String(const StringView &sv) : m_characters(sv.m_characters), m_length(sv.m_length) {}

    [[nodiscard]] size_t length() const { return m_length; }

    [[nodiscard]] const char *as_cstring() const { return const_cast<char *>(m_characters); }

private:
    const char *m_characters{nullptr};
    size_t m_length{0};
};
}// namespace Lib