// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "StackFrame.h"
#include <PhysicalAddress.h>
#include <VirtualAddress.h>

namespace Kernel {
class Process {
public:
    Process();
    [[nodiscard]] StackFrame get_stack_frame();
    [[nodiscard]] VirtualAddress get_rsp() const { return m_rsp; }
    void set_rsp(VirtualAddress rsp);
    void set_page_directory(PhysicalAddress);

    PhysicalAddress get_page_directory() const;

private:
    PhysicalAddress m_page_directory;
    VirtualAddress m_rsp;
    // Vector<Thread> m_threads;
};
}// namespace Kernel
