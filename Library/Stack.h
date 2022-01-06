// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>
#include <VirtualAddress.h>

namespace Lib {
class Stack {
public:
    Stack() = default;
    Stack(VirtualAddress address, size_t size) : m_stack(reinterpret_cast<uint64_t *>(address.as_ptr())), m_index(size / sizeof(uint64_t)) {}
    void push(uint64_t value);
    uint64_t pop();
    VirtualAddress get_stack_ptr();
private:
    uint64_t *m_stack{nullptr};
    size_t m_index{0};
};
}
