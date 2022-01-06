// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "../Heap/SlabAllocator.h"
#include "Process.h"
#include "StackFrame.h"
#include <Result.h>
#include <Vector.h>

namespace Kernel {
class ProcessManager {
public:
    static ProcessManager &get_instance() {
        static ProcessManager instance;
        return instance;
    }
    ProcessManager(ProcessManager const &) = delete;
    void operator=(ProcessManager const &) = delete;

    Result<void> initialise();
    Result<void> create_process();
    Process * current_process();
    Process * next_process();

private:
    ProcessManager() = default;
    Slab *m_allocator{nullptr};
    size_t m_current_process{0};
    Lib::Vector<Process *> process_list;
};
}// namespace Kernel
