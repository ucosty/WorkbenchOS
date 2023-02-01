// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "LibStd/Result.h"
#include "LibStd/Types.h"

namespace Kernel {
struct CpuidResponse {
    u64 rax;
    u64 rbx;
    u64 rcx;
    u64 rdx;
};

class Processor {
public:
    [[noreturn]] static void halt();
    static CpuidResponse cpuid(u64 cpuid_function);

    static bool has_apic();
    static bool has_x2apic();
    static bool tsc_deadline();

    static void disable_interrupts();
    static void enable_interrupts();
    static void load_task_register(u16 selector);

    static void interrupt();

    static u64 read_msr(u32 id);

    static u8 local_apic_id();

    static void disable_pic();
};
}// namespace Kernel
