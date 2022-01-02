// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

#define RSDP_SIGNATURE 0x2052545020445352
#define XSDT_SIGNATURE 0x54445358

namespace ACPI {
struct PACKED RootSystemDescriptionPointer {
    uint64_t signature;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[8];
};

struct PACKED ExtendedAddress {
    uint8_t address_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t address;
};

struct PACKED SystemDescriptionTableHeader {
    uint32_t signature;
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

struct PACKED MultipleApicDescriptionTable {
    uint32_t local_interrupt_controller_address;
    uint32_t flags;
};

struct PACKED MultipleApicDescriptionEntry {
    uint8_t type;
    uint8_t length;
};

struct PACKED FixedAcpiDescriptionTable {
    uint32_t firmware_ctrl;
    uint32_t dsdt;

    uint8_t reserved;

    uint8_t preferred_power_management_profile;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4_bios_req;
    uint8_t pstate_control;
    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;
    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t c_state_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    uint16_t boot_architecture_flags;

    uint8_t reserved_2;
    uint32_t flags;

    ExtendedAddress reset_reg;

    uint8_t reset_value;
    uint8_t reserved_3[3];

    uint64_t x_firmware_control;
    uint64_t x_dsdt;

    ExtendedAddress x_pm1a_event_block;
    ExtendedAddress x_pm1b_event_block;
    ExtendedAddress x_pm1a_control_block;
    ExtendedAddress x_pm1b_control_block;
    ExtendedAddress x_pm2_control_block;
    ExtendedAddress x_pm_timer_block;
    ExtendedAddress x_gpe0_block;
    ExtendedAddress x_gpe1_block;
};
}// namespace ACPI
