// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>

namespace Std {
class RefCounter {
public:
    void increment() { m_counter++; }

    void decrement() {
        if (m_counter > 0) m_counter--;
    }

    [[nodiscard]] size_t get() const { return m_counter; }

    [[nodiscard]] bool has_references() const { return m_counter > 0; }

private:
    size_t m_counter{1};
};

template<typename T>
class RefPtr {
public:
    ~RefPtr() {
        m_counter->decrement();
        if (!m_counter->has_references()) {
            delete m_ptr;
            delete m_counter;
        }
    }

    RefPtr(const RefPtr &other) : m_ptr(other.m_ptr), m_counter(other.m_counter) {
        m_counter->increment();
    }

    static RefPtr from(T *ptr) {
        return RefPtr(ptr);
    }

    static RefPtr from(const T *ptr) {
        return RefPtr(const_cast<T *>(ptr));
    }

    T *get() const { return m_ptr; }

    T *operator->() const { return m_ptr; }

private:
    explicit RefPtr(T *ptr) : m_ptr(ptr) {
        m_counter = new RefCounter();
    }

    T *m_ptr{nullptr};
    RefCounter *m_counter{nullptr};
};
}// namespace Lib
