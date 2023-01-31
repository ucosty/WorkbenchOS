// WorkbenchOS
// Copyright (c) 2023 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

enum class MutexState {
    Unlocked = 0,
    Locked = 1
};

class Mutex {
public:
    MutexState enter();
    void exit();
private:
    volatile int m_lock = 0;
};
