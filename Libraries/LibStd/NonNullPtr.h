// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Result.h>

namespace Std {
template<typename T>
class NonNullPtr {
public:
    static Result<NonNullPtr> from(T *ptr) {
        if (ptr == nullptr) {
            return Error::from_code(1);
        }
        return NonNullPtr(ptr);
    }
    [[nodiscard]] T *as_ptr() const { return m_ptr; }
    [[nodiscard]] uint64_t as_address() const { return reinterpret_cast<uint64_t>(m_ptr); }

private:
    explicit NonNullPtr(T *ptr) : m_ptr(ptr) {}
    T *m_ptr;
};
}// namespace Std
