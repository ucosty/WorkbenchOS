// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Descriptors.h>
enum class PrivilegeLevel {
    Kernel = 0x00,
    User = 0x03
};

class InterruptVectorTable {
public:
    static InterruptVectorTable &get_instance() {
        static InterruptVectorTable instance;
        return instance;
    }
    InterruptVectorTable(InterruptVectorTable const &) = delete;
    void operator=(InterruptVectorTable const &) = delete;
    void initialise();
    static void set_interrupt_gate(uint8_t id, PrivilegeLevel dpl, void (*handler)());

private:
    InterruptVectorTable() = default;
};

struct PACKED StackFrame {
    uint64_t rax;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdi;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct PACKED StackFrameErrorCode {
    uint64_t rax;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdi;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

#define PUSH_REGISTERS()       \
    asm volatile("push %rax\n" \
                 "push %rbp\n" \
                 "push %rbx\n" \
                 "push %rcx\n" \
                 "push %rdi\n" \
                 "push %rdx\n" \
                 "push %rsi\n" \
                 "push %r8\n"  \
                 "push %r9\n"  \
                 "push %r10\n" \
                 "push %r11\n" \
                 "push %r12\n" \
                 "push %r13\n" \
                 "push %r14\n" \
                 "push %r15\n")

#define POP_REGISTERS()       \
    asm volatile("pop %r15\n" \
                 "pop %r14\n" \
                 "pop %r13\n" \
                 "pop %r12\n" \
                 "pop %r11\n" \
                 "pop %r10\n" \
                 "pop %r9\n"  \
                 "pop %r8\n"  \
                 "pop %rsi\n" \
                 "pop %rdx\n" \
                 "pop %rdi\n" \
                 "pop %rcx\n" \
                 "pop %rbx\n" \
                 "pop %rbp\n" \
                 "pop %rax\n")

#define INTERRUPT_HANDLER(name)                 \
    extern "C" void name##_handler(StackFrame); \
    void NAKED name##_asm_wrapper() {           \
        PUSH_REGISTERS();                       \
        asm volatile("call " #name "_handler"); \
        POP_REGISTERS();                        \
        asm volatile("iretq");                  \
    }

#define EXCEPTION_HANDLER(name)                 \
    extern "C" void name##_handler(StackFrame); \
    void NAKED name##_asm_wrapper() {           \
        PUSH_REGISTERS();                       \
        asm volatile("call " #name "_handler"); \
        POP_REGISTERS();                        \
        asm volatile("hlt");                    \
    }

#define EXCEPTION_HANDLER_WITH_CODE(name)                \
    extern "C" void name##_handler(StackFrameErrorCode); \
    void NAKED name##_asm_wrapper() {                    \
        PUSH_REGISTERS();                                \
        asm volatile("call " #name "_handler");          \
        POP_REGISTERS();                                 \
        asm volatile("hlt");                             \
    }