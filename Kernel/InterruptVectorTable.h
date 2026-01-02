// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "Process/StackFrame.h"
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
    static void set_interrupt_gate(u8 id, PrivilegeLevel dpl, u8 ist, void (*handler)());

private:
    InterruptVectorTable() = default;
};

struct PACKED StackFrameErrorCode {
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rsi;
    u64 rdx;
    u64 rdi;
    u64 rcx;
    u64 rbx;
    u64 rbp;
    u64 rax;
    u64 interrupt;
    u64 error;
    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
};

#define PUSH_REGISTERS(interrupt)      \
    asm volatile("pushq $" #interrupt "\n" \
                 "push %rax\n"         \
                 "push %rbp\n"         \
                 "push %rbx\n"         \
                 "push %rcx\n"         \
                 "push %rdi\n"         \
                 "push %rdx\n"         \
                 "push %rsi\n"         \
                 "push %r8\n"          \
                 "push %r9\n"          \
                 "push %r10\n"         \
                 "push %r11\n"         \
                 "push %r12\n"         \
                 "push %r13\n"         \
                 "push %r14\n"         \
                 "push %r15\n"         \
                 "mov %rsp, %rdi\n")

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
                 "pop %rax\n" \
                 "addq $8, %rsp")

#define INTERRUPT_HANDLER(interrupt, name)        \
    extern "C" void name##_handler(StackFrame *); \
    void NAKED name##_asm_wrapper() {             \
        PUSH_REGISTERS(interrupt);                \
        asm volatile("call " #name "_handler");   \
        POP_REGISTERS();                          \
        asm volatile("iretq");                    \
    }

#define EXCEPTION_HANDLER(interrupt, name)        \
    extern "C" void name##_handler(StackFrame *); \
    void NAKED name##_asm_wrapper() {             \
        PUSH_REGISTERS(interrupt);                \
        asm volatile("call " #name "_handler");   \
        POP_REGISTERS();                          \
        asm volatile("hlt");                      \
    }

#define EXCEPTION_HANDLER_WITH_CODE(interrupt, name)       \
    extern "C" void name##_handler(StackFrameErrorCode *); \
    void NAKED name##_asm_wrapper() {                      \
        PUSH_REGISTERS(interrupt);                         \
        asm volatile("call " #name "_handler");            \
        POP_REGISTERS();                                   \
        asm volatile("hlt");                               \
    }