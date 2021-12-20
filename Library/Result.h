// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Optional.h>
#include <Error.h>

template<typename T, typename E>
class Result {
public:
    Result(T value) : m_value(value) {}
    Result(E error) : m_error(error), m_is_error(true) {}
    [[nodiscard]] bool is_error() const { return m_is_error; }
    E get_error() const { return m_error.get(); }
    static Result<T, E> of(T value) { return Result(value); }
    T get() const { return m_value.get(); }
private:
    // TODO: Wrap in an optional
    Optional<T> m_value;
    Optional<E> m_error;
    bool m_is_error { false };
};

template<typename E>
class Result<void, E> {
public:
    Result() {}
    Result(E error) : m_error(error), m_is_error(true) {}
    [[nodiscard]] bool is_error() const { return m_is_error; }
    E get_error() const { return m_error.get(); }
    void get() const {};
private:
    Optional<E> m_error;
    bool m_is_error { false };
};

template<typename T, typename E = Lib::Error>
class Result;
