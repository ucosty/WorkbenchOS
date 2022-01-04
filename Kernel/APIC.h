// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Result.h>
#include <Types.h>

namespace Kernel {
struct PACKED TimerVector {
    uint32_t vector : 8;         // bits 0 - 7
    uint32_t ignored : 4;        // bits 8 - 11
    uint32_t delivery_status : 1;// bits 12
    uint32_t ignored_2 : 3;      // bits 13 - 15
    uint32_t mask : 1;           // bits 16
    uint32_t timer_mode : 2;     // bits 17 - 18
    uint32_t ignored_3 : 13;     // bits 19 - 31
};
static_assert(sizeof(TimerVector) == 4);

struct PACKED SpuriousVector {
    uint32_t vector : 8;
    uint32_t apic_software_enabled : 1;
    uint32_t focus_processor_checking : 1;
    uint32_t ignored : 2;
    uint32_t eoi_broadcast_suppression : 1;
    uint32_t ignored_2 : 19;
};
static_assert(sizeof(SpuriousVector) == 4);

struct PACKED LintVector {
    uint32_t vector : 8;
    uint32_t delivery_mode : 3;
    uint32_t ignored : 1;
    uint32_t delivery_status : 1;
    uint32_t interrupt_input_pin_polarity : 1;
    uint32_t remote_irr : 1;
    uint32_t trigger_mode : 1;
    uint32_t mask : 1;
    uint32_t ignored_2 : 15;
};

static_assert(sizeof(LintVector) == 4);

class APIC {
public:
    Result<void> initialise();
    [[nodiscard]] uint64_t register_base() const { return m_register_base; };

private:
    uint64_t m_register_base{0};
};
}// namespace Kernel::APIC
