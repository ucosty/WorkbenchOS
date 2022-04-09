// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <InterruptVectorTable.h>

void configure_exceptions(InterruptVectorTable &ivt);
