// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Process.h"

namespace Kernel {
Process::Process() : m_page_directory() {
}

void Process::set_rsp(VirtualAddress rsp) {
    m_rsp = rsp;
}

void Process::set_page_directory(PhysicalAddress address) {
    m_page_directory = address;
}

}// namespace Kernel
