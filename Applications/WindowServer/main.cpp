// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/Vector.h>
#include <LibStd/Result.h>
#include <LibStd/Try.h>

namespace Kernel {
enum class DeviceType {
    GraphicsAdapter,
};

class Device {};

class GraphicsAdapter : public Device {
};

Std::Result<Std::Vector<Device *>> FindDevices(DeviceType type) {
    auto devices = Std::Vector<Device *>();
    devices.append(new GraphicsAdapter());
    return devices;
}
}

Std::Result<void> workbench_main() {
    auto graphics_cards = TRY(Kernel::FindDevices(Kernel::DeviceType::GraphicsAdapter));

    auto server = IPC::Server();


    return {};
}
