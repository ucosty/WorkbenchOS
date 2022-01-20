// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "PCI.h"
#include <ACPI/ACPI.h>
#include <Interfaces/Ports.h>

const char *pci_class_name(uint8_t class_code) {
    switch (class_code) {
        case 0:
            return "Unclassified";
        case 1:
            return "Mass Storage Controller";
        case 2:
            return "Network Controller";
        case 3:
            return "Display Controller";
        case 4:
            return "Multimedia Controller";
        case 5:
            return "Memory Controller";
        case 6:
            return "Bridge";
        case 7:
            return "Communications Controller";
        case 8:
            return "Generic System Peripheral";
        case 9:
            return "Input Device Controller";
        case 10:
            return "Docking Station";
        case 11:
            return "Processor";
        case 12:
            return "Serial Bus Controller";
        case 13:
            return "Wireless Controller";
        case 14:
            return "Intelligent Controller";
        case 15:
            return "Satellite Communications Controller";
        case 16:
            return "Encryption Controller";
        case 17:
            return "Signal Processing Controller";
        case 18:
            return "Processing Accelerators";
        case 19:
            return "Non-Essential Instrumentation";
        default:
            return "Unassigned";
    }
}

//00	Host bridge
//01	ISA bridge
//02	EISA bridge
//03	MicroChannel bridge
//04	PCI bridge
//05	PCMCIA bridge
//06	NuBus bridge
//07	CardBus bridge
//08	RACEway bridge
//09	Semi-transparent PCI-to-PCI bridge
//0a	InfiniBand to PCI host bridge
//80	Bridge

Result<void> PCI::initialise() {
    auto &acpi = Kernel::ACPI::get_instance();
    if (acpi.has_mcfg_table()) {
        // Use the MCFG table to get the ACPI configuration space
        // TODO: implement this code path when I can get it working with QEMU
        printf("PCI: using memory-mapped mechanism\n");
        return {};
    }
    printf("PCI: using io-port mechanism\n");
    return initialise_legacy();
}

Result<void> PCI::initialise_legacy() {
    for (int i = 0; i < 32; i++) {
        TRY(read_device_function(0, i, 0));
    }
    return {};
}

Result<void> PCI::read_device_function(uint8_t bus, uint8_t device, uint8_t function) {
    auto id = config_read_dword(bus, device, function, 0);
    if ((id & 0xffff) == 0xffff)
        return {};

    auto status = config_read_dword(bus, device, function, 4);
    auto info = config_read_dword(bus, device, function, 8);
    auto more_info = config_read_dword(bus, device, function, 12);
    //    auto bar0 = config_read_dword(bus, device, function, 16);
    //    auto bar1 = config_read_dword(bus, device, function, 20);
    //    auto bar2 = config_read_dword(bus, device, function, 24);
    //    auto bar3 = config_read_dword(bus, device, function, 28);
    //    auto bar4 = config_read_dword(bus, device, function, 32);
    //    auto bar5 = config_read_dword(bus, device, function, 36);
    //    auto cardbus_cis_pointer = config_read_dword(bus, device, function, 40);
    //    auto subsystem = config_read_dword(bus, device, function, 44);
    //    auto expansion_rom_base_address = config_read_dword(bus, device, function, 48);

    auto header_type = static_cast<uint8_t>((more_info >> 16) & 0xff);
    auto is_multifunction = header_type & 0x80;

    auto vendor_id = static_cast<uint32_t>(id & 0xffff);
    auto device_id = static_cast<uint32_t>(id >> 16);
    auto status_value = static_cast<uint32_t>(status >> 16);
    auto class_code = static_cast<uint32_t>((info >> 24) & 0xff);
    auto subclass = static_cast<uint32_t>((status >> 16) & 0xff);

    auto foo = new Device(vendor_id, device_id);
    m_devices.append(foo);

    printf("PCI: vendor_id = %x, device_id = %x, status = %x, class = %x, subclass = %x\n", vendor_id, device_id, status_value, class_code, subclass);
    if (is_multifunction) {
        for (int i = 1; i < 8; i++) {
            read_device_function(bus, device, i);
        }
    }

    return {};
}


uint32_t PCI::config_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    auto address = Address{
        .offset = offset,
        .function = function,
        .device_number = device,
        .bus_number = bus,
        .enable = 1};

    outl(0xcf8, address.value);
    return inl(0xcfc);
}

uint16_t PCI::config_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    auto address = Address{
        .offset = offset,
        .function = function,
        .device_number = device,
        .bus_number = bus,
        .enable = 1};

    outl(0xcf8, address.value);
    auto response = inl(0xcfc);
    return static_cast<uint16_t>(response & 0xFFFF);
}
