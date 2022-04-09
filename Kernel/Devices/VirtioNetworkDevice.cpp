// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "VirtioNetworkDevice.h"

void VirtioNetworkDevice::detect(PCI *pci) {
    auto pci_device = pci->find_device(0x1af4, 0x1000);
    if(!pci_device.is_present()) {
        return;
    }

    printf("Virtio Network Device found\n");
}
