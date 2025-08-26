// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "PS2Keyboard.h"

#include <UnbufferedConsole.h>

#include <APIC.h>

#include "Platform/x86_64/PortIO.h"

void PS2Keyboard::init() {
    // Kernel::IsaIrqRequest req{
    //     .irq        = 1,
    //     .trigger    = Kernel::TriggerMode::Edge,
    //     .polarity   = Kernel::Polarity::High,
    //     .affinity   = Kernel::Cpu::Any,
    //     .handler    = keyboard_isr,
    //     .driver_ctx = this
    // };

    // m_irq = Interrupts::register_isa_irq(req);
    // m_irq.enable();
}

void PS2Keyboard::interrupt_handler() {
    const auto key = PortIO::in8(0x60);
    if(m_buffer_items < m_buffer_size - 1) {
        m_buffer[m_buffer_items++] = key;
    }
    println("!");
}

u8 PS2Keyboard::read() {
    if(m_buffer_items > 0) {
        return m_buffer[--m_buffer_items];
    }
    return 0;
}
