// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Result.h>
#include <Types.h>

namespace Kernel {
struct CpuidResponse {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
};

class Processor {
public:
    [[noreturn]] static void halt();
    static Result<CpuidResponse> cpuid(uint64_t cpuid_function);
};
}// namespace Kernel
