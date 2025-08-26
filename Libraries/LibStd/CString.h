// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/Types.h>

extern "C" void *memset(char *buffer, int value, size_t count);
extern "C" void *memcpy(void *destination, const void *source, size_t num);
extern "C" int memcmp(const void *s1, const void *s2, size_t n);
extern "C" size_t strlen(const char *str);
[[noreturn]] void loop_de_loop();