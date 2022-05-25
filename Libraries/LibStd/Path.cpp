// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Path.h"

Path::Path(Type type) : m_type(type) {
}

Path Path::from_string(Type type, Std::String filename) {
    auto path_components = Std::StringSplitter::split(filename, '/');
    auto path = Path{type};
    if(type == Type::File) {
        path.set_filename(path_components.pop());
    }
    path.set_components(path_components);
    return path;
}

void Path::set_filename(const Std::String &filename) {
    m_filename = filename;
}
