// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include "PCI.h"
#include <ACPI/ACPI.h>
#include <Bus/Ports.h>

using namespace Std;

const char *class_name[] = {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge",
    "Communications Controller",
    "Generic System Peripheral",
    "Input Device Controller",
    "Docking Station",
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communications Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerators",
    "Non-Essential Instrumentation",
};

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
        println("PCI: using memory-mapped mechanism");
        return {};
    }
    println("PCI: using io-port mechanism");
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
    auto bar0 = config_read_dword(bus, device, function, 16);
    auto bar1 = config_read_dword(bus, device, function, 20);
    auto bar2 = config_read_dword(bus, device, function, 24);
    auto bar3 = config_read_dword(bus, device, function, 28);
    auto bar4 = config_read_dword(bus, device, function, 32);
    auto bar5 = config_read_dword(bus, device, function, 36);
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

    auto device_object = new Device(vendor_id, device_id, bar0, bar1, bar2, bar3);

    m_devices.append(device_object);

    println("PCI: vendor_id = {}, device_id = {}, status = {}, class = {}, subclass = {}, bar0 = {}", vendor_id, device_id, status_value, class_code, subclass);
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

Optional<Device *> PCI::find_device(uint16_t vendor_id, uint16_t device_id) {
    for(auto device: m_devices) {
        if(device->vendor_id() == vendor_id && device->device_id() == device_id) {
            return device;
        }
    }
    return {};
}
