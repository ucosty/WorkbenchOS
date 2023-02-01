// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#define EFICALL __attribute__((ms_abi))

#define PACKED __attribute__((__packed__))
#define NAKED __attribute__((__naked__))

using uint64_t = __UINT64_TYPE__;
using uint32_t = __UINT32_TYPE__;
using uint16_t = __UINT16_TYPE__;
using uint8_t = __UINT8_TYPE__;
using int64_t = __INT64_TYPE__;
using int32_t = __INT32_TYPE__;
using int16_t = __INT16_TYPE__;
using int8_t = __INT8_TYPE__;

using u64 = __UINT64_TYPE__;
using u32 = __UINT32_TYPE__;
using u16 = __UINT16_TYPE__;
using u8 = __UINT8_TYPE__;
using i64 = __INT64_TYPE__;
using i32 = __INT32_TYPE__;
using i16 = __INT16_TYPE__;
using i8 = __INT8_TYPE__;

using size_t = __SIZE_TYPE__;
using ptrdiff_t = __PTRDIFF_TYPE__;
using intptr_t = __INTPTR_TYPE__;
using uintptr_t = __UINTPTR_TYPE__;

template<typename T>
constexpr T &&move(T &other) {
    return static_cast<T &&>(other);
}

static_assert(sizeof(uint8_t) == 1);
static_assert(sizeof(int8_t) == 1);
static_assert(sizeof(uint16_t) == 2);
static_assert(sizeof(int16_t) == 2);
static_assert(sizeof(uint32_t) == 4);
static_assert(sizeof(int32_t) == 4);
static_assert(sizeof(uint64_t) == 8);
static_assert(sizeof(int64_t) == 8);
static_assert(sizeof(size_t) == 8);

constexpr uint64_t KiB = 1024;
constexpr uint64_t MiB = 1024 * KiB;
constexpr uint64_t GiB = 1024 * MiB;
constexpr uint64_t TiB = 1024 * GiB;
constexpr uint64_t Page = 4 * KiB;

inline void *operator new(size_t, void *ptr) noexcept {
    return ptr;
}

inline void *operator new[](size_t, void *ptr) noexcept {
    return ptr;
}
