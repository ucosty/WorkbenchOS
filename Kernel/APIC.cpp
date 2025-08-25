// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "APIC.h"
#include "UnbufferedConsole.h"
#include <ACPI/ACPI.h>
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
    explicit IOAPIC(const u64 address) : m_address(address) {}

    void route_irq(const u8 base, const u8 vector, const u8 dest_apic_id, const bool level_trigger, const bool active_low) const {
        // Program HI first: destination APIC ID
        const u32 hi = static_cast<u32>(dest_apic_id) << 24;
        // write_io_apic(m_address, base + 1, hi);
        write_register(base + 1, hi);

        // Build LO from scratch
        u32 lo = 0;
        lo |= vector;                 // [7:0] vector
        // delivery mode Fixed (000) at [10:8] => 0
        // dest mode Physical (0) at [11]      => 0
        if (active_low)    lo |= (1u << 13); // polarity
        if (level_trigger) lo |= (1u << 15); // trigger
        lo &= ~(1u << 16);                   // mask = 0 (UNMASK)

        write_register(base + 0, lo);
        println("route_irq: vector = {}, base = {}, hi = {}, lo = {}", vector, base, hi, lo);
    }

    [[nodiscard]] u32 read_register(const u32 reg) const {
        // auto volatile *registers = static_cast<u32 volatile *>(m_address);
        // registers[0] = reg & 0xff;
        // return registers[4];

        // tell IOREGSEL where we want to read from
        *reinterpret_cast<volatile uint32_t *>(reinterpret_cast<u64>(m_address)) = reg;

        // return the data from IOWIN
        return *reinterpret_cast<volatile uint32_t *>(reinterpret_cast<u64>(m_address) + 0x10);
    }

    void write_register(const u32 reg, const u32 value) const {
        // auto volatile *registers = static_cast<u32 volatile *>(m_address);
        // registers[0] = reg & 0xff;
        // registers[4] = value;

        // tell IOREGSEL where we want to write to
        *reinterpret_cast<volatile uint32_t *>(reinterpret_cast<u64>(m_address)) = reg;

        // write the value to IOWIN
        *reinterpret_cast<volatile uint32_t *>(reinterpret_cast<u64>(m_address) + 0x10) = value;
    }


private:
    u64 m_address;
};

Result<void> APIC::initialise() {
    auto &acpi = ACPI::get_instance();

    auto has_apic = Kernel::Processor::has_apic();
    if (!has_apic) {
        println("APIC not found");
        return Error::from_code(1);
    }

    println("Local APIC ID = {}", Kernel::Processor::local_apic_id());
    auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    m_register_base = apic_base_msr & ~0xfff;

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

    auto find_io_apic = [](MultipleApicDescriptionTable * header) -> Result<MadtIoApic*> {
        int count = 0;
        auto entry = reinterpret_cast<MultipleApicDescriptionEntry *>(header + 1);
        auto remainder = header->length - sizeof(MultipleApicDescriptionTable);
        auto table_bytes = reinterpret_cast<u8 *>(entry);
        while (remainder > 0) {
            entry = reinterpret_cast<MultipleApicDescriptionEntry *>(table_bytes);
            println("Entry {}: length = {}, type = {}", count++, entry->length, entry->type);

            // is IO/APIC Interrupt Source Override
            if (entry->type == 2) {
                const auto override = reinterpret_cast<MadtInterruptSourceOverride *>(entry);
                const u32 bus_source = override->bus_source;
                const u32 irq_source = override->irq_source;
                println("Override: bus = {}, irq = {}", bus_source, irq_source);
            }

            // is IO/APIC entry
            if (entry->type == 1) {
                const auto ioapic = reinterpret_cast<MadtIoApic *>(entry);
                const u32 id = ioapic->id;
                println("IO/APIC found: id = {}, address = {:#x}, GSI base = {}", id, ioapic->address, ioapic->global_system_interrupt_base);
                return ioapic;
            }

            table_bytes += entry->length;
            remainder -= entry->length;
        }

        return Error::from_code(1);
    };

    auto io_apic_address = TRY(find_io_apic(madt_header));
    auto io_apic = IOAPIC{PhysicalAddress(io_apic_address->address).as_mapped_address()};

    // Get the IO APIC version
    const auto data = io_apic.read_register(0x01);
    const auto num_redirection_entries = (data >> 15) & 0xFF;
    println("IO APIC version = {}, redirection entries = {}", data & 0xFF, num_redirection_entries);

    // 2) Figure out the keyboard GSI (default 1; override if MADT has an ISO for IRQ1)
    u32 kbd_gsi = 1;    // replace with ISO value if present
    bool kbd_level = false;  // from ISO flags if present
    bool kbd_low   = false;  // from ISO flags if present

    // 3) Compute pin and redir register base
    // u32 pin  = kbd_gsi - ioapic_gsi;
    u8 base = 0x10 + 2 * kbd_gsi;

    // 4) Program entry
    const auto local_apic_id = Kernel::Processor::local_apic_id();
    io_apic.route_irq(base, /*vector*/0x21, local_apic_id, kbd_level, kbd_low);

    asm volatile("sti");
    return {};
}

void APIC::write_icr(const ICR &icr) {
    const auto icr_high = PhysicalAddress(apic_register_base() + 0x310).as_ptr<u32>();
    const auto icr_low = PhysicalAddress(apic_register_base() + 0x300).as_ptr<u32>();
    *icr_high = icr.high();
    *icr_low = icr.low();
}

void APIC::send_init(const u8 cpu) {
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

void APIC::send_sipi(const u8 cpu) {
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
    const auto apic_base_msr = Kernel::Processor::read_msr(0x1B);
    return apic_base_msr & ~0xfff;
}
}// namespace Kernel
