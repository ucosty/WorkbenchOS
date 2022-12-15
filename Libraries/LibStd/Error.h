// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>
#include <LibStd/StringView.h>
#include <LibStd/String.h>

namespace Std {
class Error {
public:
    static Error from_code(uint64_t code) { return Error(code); }
    static Error with_message(StringView message) { return Error(message); };

    [[nodiscard]] bool is_code() const { return m_code > 0; }
    [[nodiscard]] uint64_t get_code() const { return m_code; }
    [[nodiscard]] StringView get_message() const { return m_message; }

protected:
    explicit Error(uint64_t code) : m_code(code) {}
    explicit Error(StringView message) : m_message(message) {}

private:
    uint64_t m_code{0};
    StringView m_message;
};
}// namespace Std
