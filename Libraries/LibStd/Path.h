// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/String.h>
#include <LibStd/StringSplitter.h>
#include <LibStd/Vector.h>

class Path {
public:
    enum class Type {
        File,
        Directory
    };

    static Path from_string(Type type, Std::String path);

protected:
    Path(Type type);

private:
    Std::Vector<Std::String> m_components;
    Std::String m_filename;
    Type m_type;

    void set_components(const Std::Vector<Std::String> &components);
    void set_filename(const Std::String& filename);
};
