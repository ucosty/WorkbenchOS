// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "APIC.h"
#include <ACPI/ACPI.h>
#include <ConsoleIO.h>
#include <LibStd/Try.h>
#include <PhysicalAddress.h>
#include <Processor.h>

namespace Kernel {

uint32_t read_io_apic(void *ioapicaddr, uint32_t reg) {
    auto volatile *ioapic = (uint32_t volatile *) ioapicaddr;
    ioapic[0] = (reg & 0xff);
    return ioapic[4];
}

void write_io_apic(void *ioapicaddr, uint32_t reg, uint32_t value) {
    auto volatile *ioapic = (uint32_t volatile *) ioapicaddr;
    ioapic[0] = (reg & 0xff);
    ioapic[4] = value;
}

inline uint8_t inb(unsigned int port) {
    unsigned char ret;
    asm volatile("inb %%dx,%%al"
                 : "=a"(ret)
                 : "d"(port));
    return ret;
}

void inline outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}


Std::Result<void> send_mouse_command(uint8_t command) {
    outb(0x60, command);
    //    while (!(inb(0x64) & 1)) asm("pause");
    auto ack = inb(0x60);
    if (ack != 0xfa) {
        return Std::Error::from_code(1);
    }
    return {};
}


inline void mouse_wait(uint8_t a_type)//unsigned char
{
    uint32_t _time_out = 100000;//unsigned int
    if (a_type == 0) {
        while (_time_out--)//Data
        {
            if ((inb(0x64) & 1) == 1) {
                return;
            }
        }
        return;
    } else {
        while (_time_out--)//Signal
        {
            if ((inb(0x64) & 2) == 0) {
                return;
            }
        }
        return;
    }
}

inline void mouse_write(uint8_t a_write)//unsigned char
{
    //Wait to be able to send a command
    mouse_wait(1);
    //Tell the mouse we are sending a command
    outb(0x64, 0xD4);
    //Wait for the final part
    mouse_wait(1);
    //Finally write
    outb(0x60, a_write);
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

Std::Result<void> enable_mouse() {
    uint8_t _status;//unsigned char

    //Enable the auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);

    //Enable the interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    _status = (inb(0x60) | 2);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, _status);

    //Tell the mouse to use default settings
    mouse_write(0xF6);
    mouse_read();

    //Enable the mouse
    mouse_write(0xF4);
    mouse_read();
    return {};
}

Std::Result<void> APIC::initialise() {
    auto &acpi = ACPI::get_instance();

    auto has_apic = Kernel::Processor::has_apic();
    if (!has_apic) {
        printf("APIC not found\n");
    }

    printf("Local APIC ID = %d\n", Kernel::Processor::local_apic_id());
    auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    m_register_base = apic_base_msr & ~0xfff;
    //    auto bsp = (apic_base_msr >> 8) & 1;
    //    auto enabled = (apic_base_msr >> 11) & 1;

    //    auto local_apic_id = PhysicalAddress(m_register_base + 0x20).as_ptr<uint32_t>();
    //    auto local_apic_version = PhysicalAddress(m_register_base + 0x30).as_ptr<uint32_t>();
    //    printf("Local APIC: id = %d, version = %d\n", local_apic_id, local_apic_version);

    auto lvt_timer_register = PhysicalAddress(m_register_base + 0x320).as_ptr<TimerVector>();
    auto spurious_vector_register = PhysicalAddress(m_register_base + 0xf0).as_ptr<SpuriousVector>();
    auto divide_configure_register = PhysicalAddress(m_register_base + 0x3e0).as_ptr<uint32_t>();

    auto lint0 = PhysicalAddress(m_register_base + 0x350).as_ptr<LintVector>();
    auto lint1 = PhysicalAddress(m_register_base + 0x360).as_ptr<LintVector>();

    *divide_configure_register = 0x08;

    lint0->vector = 0x30;
    lint1->vector = 0x30;

    //    lvt_timer_register->vector = 0x20;
    //    lvt_timer_register->mask = 0;
    //    lvt_timer_register->timer_mode = 1;

    spurious_vector_register->vector = 0xff;
    spurious_vector_register->eoi_broadcast_suppression = 0;
    spurious_vector_register->focus_processor_checking = 0;
    spurious_vector_register->apic_software_enabled = 1;

    // Find the MADT
    auto madt = TRY(acpi.find_table<MultipleApicDescriptionEntry>("APIC"_sv));


    auto madt_header = reinterpret_cast<MultipleApicDescriptionTable *>(madt.header);
    printf("MADT: length = %d\n", madt_header->length);


    auto entry = reinterpret_cast<MultipleApicDescriptionEntry *>(madt_header + 1);

    auto table_bytes = reinterpret_cast<uint8_t *>(entry);
    auto remainder = madt_header->length - sizeof(MultipleApicDescriptionTable);

    auto count = 0;
    uint32_t io_apic_address = 0xfec00000;

    while (remainder > 0) {
        entry = reinterpret_cast<MultipleApicDescriptionEntry *>(table_bytes);
        //        printf("Entry %d: length = %d, type = %d\n", count++, entry->length, entry->type);

        // is IO/APIC Interrupt Source Override
        if (entry->type == 2) {
            auto override = reinterpret_cast<MadtInterruptSourceOverride *>(entry);
            uint32_t bus_source = override->bus_source;
            uint32_t irq_source = override->irq_source;
            printf("Override: bus = %d, irq = %d\n", bus_source, irq_source);
        }

        // is IO/APIC entry
        if (entry->type == 1) {
            auto ioapic = reinterpret_cast<MadtIoApic *>(entry);
            io_apic_address = 0xfec00000;
            uint32_t id = ioapic->id;
            printf("IO/APIC found: id = %x, address = %x, GSI base = %x\n", id, ioapic->address, ioapic->global_system_interrupt_base);
        }

        table_bytes += entry->length;
        remainder -= entry->length;
    }


    // Set up the keyboard interrupt

    printf("io_apic_address = %x\n", io_apic_address);


    printf(">> Setting up keyboard interrupt!!\n");
    auto io_apic = PhysicalAddress(io_apic_address).as_ptr<void>();
    //    auto value = read_io_apic(io_apic, 0x12);
    //    value |= 0x20;

    // irq0 = 0x10 + 0x11
    // irq1 = 0x12 + 0x13
    // irq2 = 0x14 + 0x15
    // irq3 = 0x16 + 0x17
    // irq4 = 0x18 + 0x19
    // irq5 = 0x1A + 0x1B
    // irq6 = 0x1C + 0x1D
    // irq6 = 0x1E + 0x1F
    // irq8 = 0x20 + 0x21
    // irq9 = 0x22 + 0x23
    // irq10 = 0x24 + 0x25
    // irq11 = 0x26 + 0x27
    // irq12 = 0x28 + 0x19

    write_io_apic(io_apic, 0x12, 0x21);
    write_io_apic(io_apic, 0x13, 0);

    write_io_apic(io_apic, 0x28, 0x22);
    write_io_apic(io_apic, 0x29, 0);

    printf(">> Done!!\n");

    TRY(enable_mouse());
    asm("sti");
    volatile auto *test = PhysicalAddress(0xfee000b0).as_ptr<uint32_t>();
    *test = 0;

    return {};
}

void APIC::write_icr(const ICR &icr) {
    auto icr_high = PhysicalAddress(apic_register_base() + 0x310).as_ptr<uint32_t>();
    auto icr_low = PhysicalAddress(apic_register_base() + 0x300).as_ptr<uint32_t>();
    *icr_high = icr.high();
    *icr_low = icr.low();
}

void APIC::send_init(uint8_t cpu) {
    write_icr(ICR{
        0,
        cpu,
        DeliveryMode::INIT,
        DestinationMode::Physical,
        DestinationShorthand::NoShorthand,
        AssertLevel::Assert,
        TriggerMode::Edge,
    });
}

void APIC::send_sipi(uint8_t cpu) {
    write_icr(ICR{
        0x08,
        cpu,
        DeliveryMode::SIPI,
        DestinationMode::Physical,
        DestinationShorthand::NoShorthand,
        AssertLevel::Assert,
        TriggerMode::Edge,
    });
}

uint64_t APIC::apic_register_base() {
    auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    return apic_base_msr & ~0xfff;
}
}// namespace Kernel
