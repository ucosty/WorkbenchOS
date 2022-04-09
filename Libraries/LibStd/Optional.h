// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "LibStd/Types.h"

namespace Std {
template<typename T>
class Optional {
public:
    Optional(T value) : m_has_value(true) {
        new (m_value) T(move(value));
    }
    Optional() = default;
    ~Optional() {
        (*((T *) m_value)).~T();
    }
    [[nodiscard]] bool is_present() const { return m_has_value; }
    T &get() const {
        return *((T *) m_value);
    }

private:
    alignas(T) uint8_t m_value[sizeof(T)] = {0};
    bool m_has_value{false};
};
}// namespace Std
