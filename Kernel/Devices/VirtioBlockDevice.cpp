// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "VirtioBlockDevice.h"
#include "PhysicalAddress.h"
#include <UnbufferedConsole.h>

void VirtioBlockDevice::detect(PCI *pci) {
    auto pci_device_or_empty = pci->find_device(0x1af4, 0x1001);
    if (!pci_device_or_empty.is_present()) {
        return;
    }

    auto pci_device = pci_device_or_empty.get();

    println("Virtio Block Device found");
    println("BAR0 = {}", pci_device->bar0());
    println("BAR1 = {}", pci_device->bar1());
    println("BAR2 = {}", pci_device->bar2());
    println("BAR3 = {}", pci_device->bar3());
}
