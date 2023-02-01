// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "LibStd/Result.h"
#include "LibStd/Types.h"

namespace Kernel {
struct PACKED TimerVector {
    u32 vector : 8;         // bits 0 - 7
    u32 ignored : 4;        // bits 8 - 11
    u32 delivery_status : 1;// bits 12
    u32 ignored_2 : 3;      // bits 13 - 15
    u32 mask : 1;           // bits 16
    u32 timer_mode : 2;     // bits 17 - 18
    u32 ignored_3 : 13;     // bits 19 - 31
};
static_assert(sizeof(TimerVector) == 4);

struct PACKED SpuriousVector {
    u32 vector : 8;
    u32 apic_software_enabled : 1;
    u32 focus_processor_checking : 1;
    u32 ignored : 2;
    u32 eoi_broadcast_suppression : 1;
    u32 ignored_2 : 19;
};
static_assert(sizeof(SpuriousVector) == 4);

struct PACKED LintVector {
    u32 vector : 8;
    u32 delivery_mode : 3;
    u32 ignored : 1;
    u32 delivery_status : 1;
    u32 interrupt_input_pin_polarity : 1;
    u32 remote_irr : 1;
    u32 trigger_mode : 1;
    u32 mask : 1;
    u32 ignored_2 : 15;
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
    u8 vector;
    u32 destination;
    DeliveryMode delivery_mode;
    DestinationMode destination_mode;
    DestinationShorthand destination_shorthand;
    AssertLevel level;
    TriggerMode trigger_mode;

    [[nodiscard]] constexpr u32 high() const { return destination << 24; }
    [[nodiscard]] constexpr u32 low() const {
        return vector | delivery_mode << 8 | destination_mode << 11 | level << 14 | trigger_mode << 15 | destination_shorthand << 18;
    }
};
static_assert(ICR{0x08, 1, DeliveryMode::INIT, DestinationMode::Physical, DestinationShorthand::NoShorthand, AssertLevel::Assert, TriggerMode::Edge}.high() == 0x1000000);
static_assert(ICR{0x08, 1, DeliveryMode::INIT, DestinationMode::Physical, DestinationShorthand::NoShorthand, AssertLevel::Assert, TriggerMode::Edge}.low() == 0x4508);

class APIC {
public:
    Std::Result<void> initialise();
    [[nodiscard]] u64 register_base() const { return m_register_base; };
    static void write_icr(const ICR &icr) ;
    static void send_init(u8 cpu);
    static void send_sipi(u8 cpu);
    static u64 apic_register_base();

private:
    u64 m_register_base{0};
};
}// namespace Kernel
