// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Stack.h"

namespace Std {
void Stack::push(uint64_t value) {
    m_stack[--m_index] = value;
}
uint64_t Stack::pop() {
    return m_stack[m_index++];
}
VirtualAddress Stack::get_stack_ptr() {
    auto address = reinterpret_cast<uint64_t>(&m_stack[m_index]);
    return VirtualAddress(address);
}
}