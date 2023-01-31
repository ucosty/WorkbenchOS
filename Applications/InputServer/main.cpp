// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/Result.h>
#include <LibStd/Try.h>
#include <LibStd/Vector.h>

namespace IPC {
class Client {
};

class Server {
};
}// namespace IPC

class KeyboardEvent {
};

enum class DeviceType {
    Keyboard,
    Mouse,
};

namespace Kernel {
class Device {};

class KeyboardDevice : public Device {
};

Std::Result<Std::Vector<Device *>> FindDevices(DeviceType type) {
    auto devices = Std::Vector<Device *>();
    devices.append(new KeyboardDevice());
    return devices;
}
}// namespace Kernel

Std::Result<void> workbench_main() {

    auto keyboards = TRY(Kernel::FindDevices(DeviceType::Keyboard));
    auto multiplexer = IPC::Server();

    for (auto &keyboard: keyboards) {
        TRY(create_server(keyboard, multiplexer));
    }

    return {};
}
