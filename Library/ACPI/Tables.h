// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <LibStd/Types.h>

#define RSDP_SIGNATURE 0x2052545020445352
#define XSDT_SIGNATURE 0x54445358

namespace Std {
struct PACKED RootSystemDescriptionPointer {
    u64 signature;
    u8 checksum;
    u8 oem_id[6];
    u8 revision;
    u32 rsdt_address;
    u32 length;
    u64 xsdt_address;
    u8 extended_checksum;
    u8 reserved[8];
};

struct PACKED ExtendedAddress {
    u8 address_space;
    u8 bit_width;
    u8 bit_offset;
    u8 access_size;
    u64 address;
};

struct PACKED SystemDescriptionTableHeader {
    u32 signature;
    u32 length;
    u8 revision;
    u8 checksum;
    u8 oem_id[6];
    u8 oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
};
static_assert(sizeof(SystemDescriptionTableHeader) == 36);

struct PACKED MCFGTable {
};

struct PACKED MultipleApicDescriptionTable {
    u32 signature;
    u32 length;
    u8 revision;
    u8 checksum;
    u8 oem_id[6];
    u8 oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
    u32 local_interrupt_controller_address;
    u32 flags;
};

struct PACKED MultipleApicDescriptionEntry {
    u8 type;
    u8 length;
};

struct PACKED MadtLocalApicEntry {
    u8 type;
    u8 length;
    u8 processor_id;
    u8 apic_id;
    u32 flags;
};

struct PACKED MadtIoApic {
    u8 type;
    u8 length;
    u8 id;
    u8 _reserved;
    u32 address;
    u32 global_system_interrupt_base;
};

struct PACKED MadtInterruptSourceOverride {
    u8 type;
    u8 length;
    u8 bus_source;
    u8 irq_source;
    u32 global_system_interrupt;
    u16 flags;
};


struct PACKED FixedAcpiDescriptionTable {
    u32 firmware_ctrl;
    u32 dsdt;

    u8 reserved;

    u8 preferred_power_management_profile;
    u16 sci_interrupt;
    u32 smi_command_port;
    u8 acpi_enable;
    u8 acpi_disable;
    u8 s4_bios_req;
    u8 pstate_control;
    u32 pm1a_event_block;
    u32 pm1b_event_block;
    u32 pm1a_control_block;
    u32 pm1b_control_block;
    u32 pm2_control_block;
    u32 pm_timer_block;
    u32 gpe0_block;
    u32 gpe1_block;
    u8 pm1_event_length;
    u8 pm1_control_length;
    u8 pm2_control_length;
    u8 pm_timer_length;
    u8 gpe0_length;
    u8 gpe1_length;
    u8 gpe1_base;
    u8 c_state_control;
    u16 worst_c2_latency;
    u16 worst_c3_latency;
    u16 flush_size;
    u16 flush_stride;
    u8 duty_offset;
    u8 duty_width;
    u8 day_alarm;
    u8 month_alarm;
    u8 century;

    u16 boot_architecture_flags;

    u8 reserved_2;
    u32 flags;

    ExtendedAddress reset_reg;

    u8 reset_value;
    u8 reserved_3[3];

    u64 x_firmware_control;
    u64 x_dsdt;

    ExtendedAddress x_pm1a_event_block;
    ExtendedAddress x_pm1b_event_block;
    ExtendedAddress x_pm1a_control_block;
    ExtendedAddress x_pm1b_control_block;
    ExtendedAddress x_pm2_control_block;
    ExtendedAddress x_pm_timer_block;
    ExtendedAddress x_gpe0_block;
    ExtendedAddress x_gpe1_block;
};
}// namespace SystemDescriptorTables
