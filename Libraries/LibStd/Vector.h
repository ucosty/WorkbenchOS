// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/CString.h>
#include <LibStd/Iterator.h>
#include <LibStd/Types.h>
#include <UnbufferedConsole.h>

namespace Std {
template<typename T>
class Vector {
public:
    Vector() : m_capacity(8), m_items(0) {
        m_storage = new uint8_t[m_capacity * sizeof(T)];
    }

    Vector(const Vector &other) {
        m_items = other.m_items;
        m_capacity = other.m_capacity;
        m_storage = new uint8_t[m_capacity * sizeof(T)];
        for(int i = 0; i < m_items; i++) {
            new(item(i)) T(other[i]);
        }
    }

    explicit Vector(size_t capacity) : m_capacity(capacity) {
        m_storage = new uint8_t[capacity * sizeof(T)];
    }

    ~Vector() {
        for (int i = 0; i < m_items; i++) {
            item(i)->~T();
        }
        delete[] m_storage;
    }

    void append(const T &value) {
        append(T(value));
    }

    void append(const T &&value) {
        if (m_items == m_capacity) {
            auto new_capacity = m_capacity == 0 ? 1 : m_capacity * 2;
            grow(new_capacity);
        }

        new (item(m_items)) T(value);
        m_items++;
    }

    T pop() {
        auto value = *item(m_items - 1);
        m_items--;
        return value;
    }

    void grow(size_t new_capacity) {
        auto *new_storage = new uint8_t[new_capacity * sizeof(T)];
        if (m_storage != nullptr) {
            memcpy(new_storage, m_storage, m_items * sizeof(T));
            delete[] m_storage;
        }
        m_capacity = new_capacity;
        m_storage = new_storage;
    }

    T &operator[](size_t index) const {
        return *item(index);
    }

    T *item(size_t index) const { return reinterpret_cast<T *>(&m_storage[index * sizeof(T)]); }

    [[nodiscard]] Iterator<T> begin() const { return item(0); }

    [[nodiscard]] Iterator<T> end() const { return item(m_items); }

    [[nodiscard]] size_t length() const { return m_items; }

private:
    size_t m_capacity{0};
    size_t m_items{0};
    uint8_t *m_storage{nullptr};
};
}// namespace Std
