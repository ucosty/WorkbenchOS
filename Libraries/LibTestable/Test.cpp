// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibStd/Format.h>
#include <LibTestable/Test.h>

void TestRunner::print_summary() {
    Std::println("\nSummary:");
    Std::println("Passed: {}, Skipped: {}, Failed: {}", m_results.passed, m_results.skipped, m_results.failed);

    if (m_results.failed > 0) {
        Std::println("\nTests Failed:");
        for (const auto &test: m_tests) {
            if (test->is_failed()) {
                Std::println("{}", test->name());
            }
        }
    }
}

void TestRunner::fail_current_test() {
    m_current_test->fail_test();
}

Std::StringView to_string(TestState state) {
    switch (state) {
        case TestState::Skipped:
            return "SKIP"_sv;
        case TestState::Pending:
            return "PEND"_sv;
        case TestState::Failed:
            return "FAIL"_sv;
        case TestState::Passed:
            return "PASS"_sv;
    }
}

int TestRunner::execute() {
    Std::println("Running {} of {} tests:", m_tests.length(), m_tests.length());
    for (const auto &test: m_tests) {
        m_current_test = test;
        auto test_result = m_current_test->execute();
        Std::println("[{}] {}", to_string(test_result), test->name());
        m_results.add_result(test_result);
    }

    print_summary();
    if (m_results.failed > 0) {
        return 1;
    }
    return 0;
}

void TestRunner::add_test(Test *test) {
    m_tests.append(test);
}
