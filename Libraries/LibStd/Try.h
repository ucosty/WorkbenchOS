// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#define TRY(expression)                  \
    ({                                   \
        const auto response = (expression);    \
        if (response.is_error())         \
            return response.get_error(); \
        response.get();                  \
    })

#define TRY_INTO(OuterErrorType, expr) \
    ({                                \
        const auto res = (expr);  \
        if (res.is_error()) { \
            return Std::convert_error<OuterErrorType>(res.get_error()); \
        } \
        res.get(); \
    })


#define TRY_PANIC(expression)            \
    ({                                   \
        auto response = (expression);    \
        if (response.is_error())         \
            panic(response.get_error()); \
        response.get();                  \
    })
