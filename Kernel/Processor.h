// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "LibStd/Result.h"
#include "LibStd/Types.h"

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
    static CpuidResponse cpuid(uint64_t cpuid_function);

    static bool has_apic();
    static bool has_x2apic();
    static bool tsc_deadline();

    static void disable_interrupts();
    static void enable_interrupts();
    static void load_task_register(uint16_t selector);

    static void interrupt();

    static uint64_t read_msr(uint32_t id);

    static uint8_t local_apic_id();

    static void disable_pic();
};
}// namespace Kernel
