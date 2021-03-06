// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <Processor.h>

namespace __cxxabiv1 {
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release(__guard *);
extern "C" void __cxa_guard_abort(__guard *);

extern "C" int __cxa_guard_acquire(__guard *g) {
    return !*(char *) (g);
}

extern "C" void __cxa_guard_release(__guard *g) {
    *(char *) g = 1;
}

extern "C" void __cxa_guard_abort(__guard *) {
}

extern "C" void __dso_handle() {
}

extern "C" void __cxa_pure_virtual() {
    Kernel::Processor::halt();
}

extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *__dso_handle) {
    return 0;
}
}// namespace __cxxabiv1
