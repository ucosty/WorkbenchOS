// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Error.h>
#include <LibStd/Optional.h>

namespace Std {
template<typename T, typename E>
class Result {
public:
    Result(T value) : m_value(move(value)) {}
    Result(E error) : m_error(move(error)) {}
    [[nodiscard]] bool is_error() const { return m_error.is_present(); }
    E get_error() const { return m_error.get(); }
    static Result<T, E> of(T value) { return Result(value); }
    T get() const { return m_value.get(); }

private:
    Optional<T> m_value;
    Optional<E> m_error;
};

template<typename E>
class Result<void, E> {
public:
    Result() {}
    Result(E error) : m_error(error) {}
    [[nodiscard]] bool is_error() const { return m_error.is_present(); }
    E get_error() const { return m_error.get(); }
    void get() const {};

private:
    Optional<E> m_error;
};

template<typename T, typename E = Std::Error>
class Result;
}// namespace Std
