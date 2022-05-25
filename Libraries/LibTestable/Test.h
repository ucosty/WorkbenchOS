// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <LibStd/StringView.h>
#include <LibStd/Vector.h>

#define VERIFY(expression)                   \
    ({                                       \
        if (!(expression)) {                 \
            test_runner->fail_current_test(); \
        }                                    \
    })

#define VERIFY_EQUALS(expression, value)                                                                                     \
    ({                                                                                                                       \
        auto response = (expression);                                                                                        \
        if ((expression) != (value)) {                                                                                       \
            fmt::print("VERIFY_EQUALS failed: " #expression " == {}, expected " #value " on line {}\n", response, __LINE__); \
            test_runner->fail_current_test();                                                                                 \
        }                                                                                                                    \
    })

typedef void (*TestFunction)();

enum class TestState {
    Pending,
    Passed,
    Failed,
    Skipped
};

class Test {
public:
    Test(TestFunction function, const char *name) : m_function(function), m_name(name) {}

    TestState execute() {
        m_function();
        if (m_state == TestState::Pending) {
            m_state = TestState::Passed;
        }
        return m_state;
    }

    void fail_test() {
        m_state = TestState::Failed;
    }

    [[nodiscard]] bool is_failed() const { return m_state == TestState::Failed; }

    [[nodiscard]] const char *name() const { return m_name; }

private:
    const char *m_name;
    TestFunction m_function;
    TestState m_state{TestState::Pending};
};

struct TestResults {
    int passed = 0;
    int skipped = 0;
    int failed = 0;

    void add_result(TestState state) {
        switch (state) {
            case TestState::Passed:
                passed++;
                break;
            case TestState::Failed:
                failed++;
                break;
            case TestState::Pending:
            case TestState::Skipped:
                skipped++;
                break;
        }
    }
};

class TestRunner {
public:
    static TestRunner &get_instance() {
        static TestRunner instance;
        return instance;
    }
    TestRunner(TestRunner const &) = delete;
    void operator=(TestRunner const &) = delete;

    void add_test(Test *test);
    int execute();
    void fail_current_test();
    void print_summary();

private:
    TestRunner() = default;
    Std::Vector<Test *> m_tests;
    Test *m_current_test{nullptr};
    TestResults m_results{};
};

#define TEST(function)                                                                \
    void function();                                                                  \
    Test function##_wrapper(function, #function);                                     \
    struct function##_injector {                                                      \
        function##_injector() noexcept { test_runner->add_test(&function##_wrapper); } \
    };                                                                                \
    struct function##_injector function##_injector;                                   \
    void function()
