// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/String.h>
#include <LibStd/Vector.h>

namespace Std::StringSplitter {
Std::Vector<Std::String> split(Std::String &, char);
}
