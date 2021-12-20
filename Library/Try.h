// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#define TRY(expression)                  \
    ({                                   \
        auto response = (expression);    \
        if (response.is_error())         \
            return response.get_error(); \
        response.get();                  \
    })