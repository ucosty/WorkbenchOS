// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "APIC.h"
#include "Processor.h"
#include <Try.h>
#include <PhysicalAddress.h>
#include <ConsoleIO.h>

namespace Kernel {
Result<void> APIC::initialise() {
    auto has_apic = Kernel::Processor::has_apic();
    if (!has_apic) {
        printf("APIC not found\n");
    }
//    printf("Local APIC ID = %d\n", TRY(Kernel::Processor::local_apic_id()));
    auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    m_register_base = apic_base_msr & ~0xfff;
//    auto bsp = (apic_base_msr >> 8) & 1;
//    auto enabled = (apic_base_msr >> 11) & 1;

//    auto local_apic_id = PhysicalAddress(m_register_base + 0x20).as_ptr<uint32_t>();
//    auto local_apic_version = PhysicalAddress(m_register_base + 0x30).as_ptr<uint32_t>();

    auto lvt_timer_register = PhysicalAddress(m_register_base + 0x320).as_ptr<TimerVector>();
    auto spurious_vector_register = PhysicalAddress(m_register_base + 0xf0).as_ptr<SpuriousVector>();
    auto divide_configure_register = PhysicalAddress(m_register_base + 0x3e0).as_ptr<uint32_t>();

    auto lint0 = PhysicalAddress(m_register_base + 0x350).as_ptr<LintVector>();
    auto lint1 = PhysicalAddress(m_register_base + 0x360).as_ptr<LintVector>();

    *divide_configure_register = 0x08;

    lint0->vector = 0x30;
    lint1->vector = 0x30;

    lvt_timer_register->vector = 0x20;
    lvt_timer_register->mask = 0;
    lvt_timer_register->timer_mode = 1;

    spurious_vector_register->vector = 0xff;
    spurious_vector_register->eoi_broadcast_suppression = 0;
    spurious_vector_register->focus_processor_checking = 0;
    spurious_vector_register->apic_software_enabled = 1;

    return {};
}
}// namespace Kernel::APIC
