// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "Processor.h"
#include <Try.h>

namespace Kernel {
[[noreturn]] void Processor::halt() {
    asm volatile("hlt");
    while (true) {}
}

CpuidResponse Processor::cpuid(uint64_t cpuid_function) {
    auto response = CpuidResponse();
    asm volatile("cpuid"
                 : "=a"(response.rax), "=b"(response.rbx), "=c"(response.rcx), "=d"(response.rdx)
                 : "a"(cpuid_function));
    return response;
}

bool Processor::has_apic() {
    auto feature_bits = cpuid(1);
    return (feature_bits.rdx >> 9) & 1;
}

bool Processor::has_x2apic() {
    auto feature_bits = cpuid(1);
    return (feature_bits.rcx >> 21) & 1;
}

bool Processor::tsc_deadline() {
    auto feature_bits = cpuid(1);
    return (feature_bits.rcx >> 24) & 1;
}

uint8_t Processor::local_apic_id() {
    auto feature_bits = cpuid(1);
    return (feature_bits.rbx >> 24) & 0xFF;
}

uint64_t Processor::read_msr(uint32_t id) {
    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"(id));
    return ((uint64_t) high << 32) | low;
}

void Processor::disable_pic() {
    asm volatile("mov $0xff, %al\n"
                 "out %al, $0xa1\n"
                 "out %al, $0x21");
}

void Processor::interrupt() {
    asm volatile("int $0x22");
}

}// namespace Kernel
