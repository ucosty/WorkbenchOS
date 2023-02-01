// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "APIC.h"
#include "UnbufferedConsole.h"
#include <ACPI/ACPI.h>
#include <ConsoleIO.h>
#include <LibStd/Try.h>
#include <PhysicalAddress.h>
#include <Processor.h>

namespace Kernel {

u32 read_io_apic(void *ioapicaddr, u32 reg) {
    auto volatile *ioapic = (u32 volatile *) ioapicaddr;
    ioapic[0] = (reg & 0xff);
    return ioapic[4];
}

void write_io_apic(void *ioapicaddr, u32 reg, u32 value) {
    auto volatile *ioapic = (u32 volatile *) ioapicaddr;
    ioapic[0] = (reg & 0xff);
    ioapic[4] = value;
}

inline u8 inb(unsigned int port) {
    unsigned char ret;
    asm volatile("inb %%dx,%%al"
                 : "=a"(ret)
                 : "d"(port));
    return ret;
}

void inline outb(u16 port, u8 val) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}


class IOAPIC {
public:
    IOAPIC(void *address) : m_address(address) {}
    void route_irq(u8 base, u8 irq_number) {
        auto value = read_io_apic(m_address, base) & 0xffffff00;
        value |= irq_number;
        write_io_apic(m_address, base, value);
        write_io_apic(m_address, base + 1, 0);
    }

private:
    void *m_address;
};

Std::Result<void> APIC::initialise() {
    auto &acpi = ACPI::get_instance();

    auto has_apic = Kernel::Processor::has_apic();
    if (!has_apic) {
        println("APIC not found");
    }

    println("Local APIC ID = {}", Kernel::Processor::local_apic_id());
    auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    m_register_base = apic_base_msr & ~0xfff;
    //    auto bsp = (apic_base_msr >> 8) & 1;
    //    auto enabled = (apic_base_msr >> 11) & 1;

    //    auto local_apic_id = PhysicalAddress(m_register_base + 0x20).as_ptr<u32>();
    //    auto local_apic_version = PhysicalAddress(m_register_base + 0x30).as_ptr<u32>();
    //    printf("Local APIC: id = %d, version = %d\n", local_apic_id, local_apic_version);

    auto lvt_timer_register = PhysicalAddress(m_register_base + 0x320).as_ptr<TimerVector>();
    auto spurious_vector_register = PhysicalAddress(m_register_base + 0xf0).as_ptr<SpuriousVector>();
    auto divide_configure_register = PhysicalAddress(m_register_base + 0x3e0).as_ptr<u32>();

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
    println("MADT: length = {}", madt_header->length);


    auto entry = reinterpret_cast<MultipleApicDescriptionEntry *>(madt_header + 1);

    auto table_bytes = reinterpret_cast<u8 *>(entry);
    auto remainder = madt_header->length - sizeof(MultipleApicDescriptionTable);

    auto count = 0;
    u32 io_apic_address = 0xfec00000;

    while (remainder > 0) {
        entry = reinterpret_cast<MultipleApicDescriptionEntry *>(table_bytes);
        //        printf("Entry %d: length = %d, type = %d\n", count++, entry->length, entry->type);

        // is IO/APIC Interrupt Source Override
        if (entry->type == 2) {
            auto override = reinterpret_cast<MadtInterruptSourceOverride *>(entry);
            u32 bus_source = override->bus_source;
            u32 irq_source = override->irq_source;
            println("Override: bus = {}, irq = {}", bus_source, irq_source);
        }

        // is IO/APIC entry
        if (entry->type == 1) {
            auto ioapic = reinterpret_cast<MadtIoApic *>(entry);
            io_apic_address = 0xfec00000;
            u32 id = ioapic->id;
            println("IO/APIC found: id = {}, address = {:x}, GSI base = {:x}", id, ioapic->address, ioapic->global_system_interrupt_base);
        }

        table_bytes += entry->length;
        remainder -= entry->length;
    }

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
    auto io_apic = IOAPIC{PhysicalAddress(io_apic_address).as_ptr<void>()};
    io_apic.route_irq(0x12, 0x21);
    io_apic.route_irq(0x28, 0x22);
    asm volatile("sti");
    return {};
}

void APIC::write_icr(const ICR &icr) {
    auto icr_high = PhysicalAddress(apic_register_base() + 0x310).as_ptr<u32>();
    auto icr_low = PhysicalAddress(apic_register_base() + 0x300).as_ptr<u32>();
    *icr_high = icr.high();
    *icr_low = icr.low();
}

void APIC::send_init(u8 cpu) {
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

void APIC::send_sipi(u8 cpu) {
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

u64 APIC::apic_register_base() {
    auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    return apic_base_msr & ~0xfff;
}
}// namespace Kernel
