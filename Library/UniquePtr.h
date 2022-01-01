// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

namespace Lib {
template<typename T>
class UniquePtr {
public:
    explicit UniquePtr(T *ptr) : m_ptr(ptr) {}

    UniquePtr(UniquePtr &) = delete;

    UniquePtr(UniquePtr &&other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }

    ~UniquePtr() {
        delete m_ptr;
    }

    T *get() const { return m_ptr; }

    T *operator->() const { return m_ptr; }

private:
    T *m_ptr;
};

template<typename T, class... Args>
static UniquePtr<T> make_unique(Args... args) {
    return UniquePtr<T>(new T(args...));
}
}// namespace Lib
