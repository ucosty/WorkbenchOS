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

enum DeliveryMode {
    Fixed = 0x0,
    LowPriority = 0x1,
    SMI = 0x2,
    NMI = 0x4,
    INIT = 0x5,
    SIPI = 0x6,
};

enum DestinationMode {
    Physical = 0x0,
    Logical = 0x1,
};

enum DestinationShorthand {
    NoShorthand = 0x0,
    Self = 0x1,
    AllIncludingSelf = 0x2,
    AllExcludingSelf = 0x3,
};

enum AssertLevel {
    Deassert = 0x0,
    Assert = 0x1
};

enum TriggerMode {
    Edge = 0x0,
    Level = 0x1,
};

struct ICR {
    uint8_t vector;
    uint32_t destination;
    DeliveryMode delivery_mode;
    DestinationMode destination_mode;
    DestinationShorthand destination_shorthand;
    AssertLevel level;
    TriggerMode trigger_mode;

    [[nodiscard]] constexpr uint32_t high() const { return destination << 24; }
    [[nodiscard]] constexpr uint32_t low() const {
        return vector | delivery_mode << 8 | destination_mode << 11 | level << 14 | trigger_mode << 15 | destination_shorthand << 18;
    }
};
static_assert(ICR{0x08, 1, DeliveryMode::INIT, DestinationMode::Physical, DestinationShorthand::NoShorthand, AssertLevel::Assert, TriggerMode::Edge}.high() == 0x1000000);
static_assert(ICR{0x08, 1, DeliveryMode::INIT, DestinationMode::Physical, DestinationShorthand::NoShorthand, AssertLevel::Assert, TriggerMode::Edge}.low() == 0x4508);

class APIC {
public:
    Result<void> initialise();
    [[nodiscard]] uint64_t register_base() const { return m_register_base; };
    static void write_icr(const ICR &icr) ;
    static void send_init(uint8_t cpu);
    static void send_sipi(uint8_t cpu);
    static uint64_t apic_register_base();

private:
    uint64_t m_register_base{0};
};
}// namespace Kernel
