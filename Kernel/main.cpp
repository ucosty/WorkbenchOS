// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <BootState.h>
#include <LinearFramebuffer.h>
#include <Types.h>

extern "C" [[noreturn]] __attribute__((ms_abi)) void kernel_main(BootState *boot_state) {
    auto framebuffer = LinearFramebuffer(boot_state->kernel_address_space.framebuffer.virtual_base, 1280, 1024);
    framebuffer.rect(50, 50, 100, 100, 0xff0000, true);
    for (;;) {}
}
