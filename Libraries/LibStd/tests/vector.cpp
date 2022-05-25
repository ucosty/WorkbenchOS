// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibTestable/Test.h>

auto test_runner = &TestRunner::get_instance();

TEST(is_true) {
    VERIFY(true);
}

extern "C" int main(int argc, char *argv[]) {
    return test_runner->execute();
}
