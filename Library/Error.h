// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

namespace Lib {
    class Error {
    public:
        static Error from_code(uint64_t code) { return Error(code); }

        [[nodiscard]] bool is_code() const { return m_code > 0; }
        [[nodiscard]] uint64_t get_code() const { return m_code; }

    protected:
        explicit Error(uint64_t code) : m_code(code) {}

    private:
        uint64_t m_code{0};
    };
}// namespace Lib
