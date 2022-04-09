// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

namespace Std {
template<typename T>
class Iterator {
public:
    Iterator(T *ptr) : m_ptr(ptr) {}
    T &operator*() const { return *m_ptr; }
    T *operator->() { return m_ptr; }
    Iterator &operator++() {
        m_ptr++;
        return *this;
    }
    Iterator operator++(int) {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }
    friend bool operator==(const Iterator &a, const Iterator &b) { return a.m_ptr == b.m_ptr; }
    friend bool operator!=(const Iterator &a, const Iterator &b) { return a.m_ptr != b.m_ptr; }

private:
    T *m_ptr;
};
}// namespace Std
