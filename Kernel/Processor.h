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

    static Result<bool> has_apic();
    static Result<bool> has_x2apic();
    static Result<bool> tsc_deadline();
    static void interrupt();

    static uint64_t read_msr(uint32_t id);

    static Result<uint8_t> local_apic_id();

    static void disable_pic();
};
}// namespace Kernel
