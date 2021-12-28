// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Processor.h"

namespace Kernel {
[[noreturn]] void Processor::halt() {
    asm volatile("hlt");
    while (true) {}
}

Result<CpuidResponse> Processor::cpuid(uint64_t cpuid_function) {
    auto response = CpuidResponse();
    asm volatile("cpuid"
                 : "=a"(response.rax), "=b"(response.rbx), "=c"(response.rcx), "=d"(response.rdx)
                 : "a"(cpuid_function));
    return response;
}
}// namespace Kernel
