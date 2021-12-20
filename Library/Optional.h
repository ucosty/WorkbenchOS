// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

template<typename T>
class Optional {
public:
    Optional(T value) : m_has_value(true) {
        *((T *) m_value) = value;
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
    alignas(T) uint8_t m_value[sizeof(T)] = { 0 };
    bool m_has_value { false };
};
