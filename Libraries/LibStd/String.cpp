// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ConsoleIO.h"
#include <LibStd/String.h>

namespace Std {
String::~String() {
    delete[] m_characters;
}

String::String(const String &other) {
    m_length = other.m_length;
    m_characters = new char[other.m_length];
    memcpy((void *) m_characters, other.m_characters, other.m_length);
}
}
