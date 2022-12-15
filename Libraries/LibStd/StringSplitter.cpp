// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "StringSplitter.h"

namespace Std::StringSplitter {
Std::Vector<Std::String> split(Std::String &string, char c) {
    using namespace Std;

    Vector<String> result;
    auto substring_start = 0;
    for (auto i = 0; i < string.length(); i++) {
        if (string[i] == c) {
            auto length = i - substring_start;
            result.append(String(&string[substring_start], length));
            substring_start = i + 1;
        }
    }

    auto length = string.length() - substring_start;
    result.append(String(&string[substring_start], length));

    return result;
}
}// namespace Std::StringSplitter
