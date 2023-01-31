// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Mutex.h"

void lock() {

}

MutexState Mutex::enter() {
    int expected = 0;
    __atomic_compare_exchange_n(&m_lock, &expected, 1, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
    return static_cast<MutexState>(expected);
}

void Mutex::exit() {
    m_lock = 0;
}
