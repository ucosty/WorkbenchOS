// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <LinearFramebuffer.h>



// TODO: Set up paging and jump into 0xffffffff80000000 + kernel_stage2
extern "C" [[noreturn]] __attribute__((ms_abi)) void kernel_main(uint64_t framebuffer_address) {
    auto framebuffer = LinearFramebuffer(framebuffer_address, 1280, 1024);



    for(;;) {}
}

// Jump into this after basic paging setup
void kernel_stage2() {
}
