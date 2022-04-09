// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "VirtioBlockDevice.h"
#include "PhysicalAddress.h"
#include <ConsoleIO.h>

void VirtioBlockDevice::detect(PCI *pci) {
    auto pci_device_or_empty = pci->find_device(0x1af4, 0x1001);
    if(!pci_device_or_empty.is_present()) {
        return;
    }

    auto pci_device = pci_device_or_empty.get();

    printf("Virtio Block Device found\n");
    printf("BAR0 = %x\n", pci_device->bar0());
    printf("BAR1 = %x\n", pci_device->bar1());
    printf("BAR2 = %x\n", pci_device->bar2());
    printf("BAR3 = %x\n", pci_device->bar3());
}
