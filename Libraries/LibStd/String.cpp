// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/CString.h>
#include <LibStd/String.h>

namespace Std {
String::~String() {
    delete[] m_characters;
}

String::String(const char *characters) {
    m_length = strlen(characters);
    m_characters = new char[m_length];
    memcpy((void *) m_characters, characters, m_length);
}

String::String(const StringView &sv) : m_length(sv.m_length) {
    m_characters = new char[m_length];
    memcpy((void *) m_characters, sv.m_characters, m_length);
}

String::String(const char *characters, size_t length) : m_length(length) {
    m_characters = new char[m_length];
    memcpy((void *) m_characters, characters, m_length);
}

String::String(String const &other) {
    m_length = other.m_length;
    m_characters = new char[m_length];

    if(m_characters == nullptr || other.m_characters == nullptr) {
        m_length = 0;
        m_characters = nullptr;
        return;
    }

    memcpy((void *) m_characters, other.m_characters, other.m_length);
}

}// namespace Std