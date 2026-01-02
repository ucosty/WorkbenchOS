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
    static Result of(T value) { return Result(value); }
    T get() const { return m_value.get(); }

    static Result error(E error) { return Result(error); }

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
    static void get() {};

private:
    Optional<E> m_error;
};

template<typename T, typename E = Error>
class Result;

template <class T> struct RemoveRef      { using Type = T; };
template <class T> struct RemoveRef<T&>  { using Type = T; };
template <class T> struct RemoveRef<T&&> { using Type = T; };

template <class T> struct RemoveCV               { using Type = T; };
template <class T> struct RemoveCV<const T>      { using Type = T; };
template <class T> struct RemoveCV<volatile T>   { using Type = T; };
template <class T> struct RemoveCV<const volatile T> { using Type = T; };

template <class T>
struct RemoveCVRef {
    using Type = typename RemoveCV<typename RemoveRef<T>::Type>::Type;
};

template <class T>
using RemoveCVRefT = typename RemoveCVRef<T>::Type;

template <class To, class From>
struct FromError; // primary template (specialize this)

template <class To, class From>
concept HasFromError = requires(RemoveCVRefT<From> f) {
    FromError<To, RemoveCVRefT<From>>::from(static_cast<RemoveCVRefT<From>&&>(f));
};

template <class To, class From>
concept BraceConstructible = requires(From&& f) {
    To{ static_cast<From&&>(f) };
};

template <class To, class From>
constexpr To convert_error(From&& from) {
    using DFrom = RemoveCVRefT<From>;
    if constexpr (HasFromError<To, From>) {
        // Note: we pass a DFrom (decayed) to the FromError impl
        return FromError<To, DFrom>::from(static_cast<DFrom&&>(from));
    } else if constexpr (BraceConstructible<To, From>) {
        return To{ static_cast<From&&>(from) };
    } else {
        static_assert(sizeof(To) == 0,
          "No error conversion available. Provide FromError<To,From>::from(...) "
          "or make To brace-constructible from From.");
    }
}

}// namespace Std
