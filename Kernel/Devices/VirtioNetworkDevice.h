// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <Interfaces/PCI.h>

class VirtioNetworkDevice {
public:
    static void detect(PCI *pci);

private:

};
