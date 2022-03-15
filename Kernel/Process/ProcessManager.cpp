// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ProcessManager.h"
#include "../Memory/MemoryManager.h"
#include "Stack.h"
#include <ConsoleIO.h>

namespace Kernel {
Result<void> ProcessManager::initialise() {
    auto &slab_allocator = SlabAllocator::get_instance();
    m_allocator = TRY(slab_allocator.get_or_create_slab(sizeof(Process))).as_ptr();

    // Create the kernel process
    auto &memory_manager = MemoryManager::get_instance();
    auto *process = TRY(m_allocator->allocate<Process>());
    process_list.append(process);

    return {};
}

extern "C" void *user_text_start;
extern "C" void *user_text_end;

Result<void> ProcessManager::create_process() {
    auto &memory_manager = MemoryManager::get_instance();
    auto *process = TRY(m_allocator->allocate<Process>());

    auto stack = TRY(memory_manager.allocate_kernel_heap_page());
    StackFrame initial_stack_frame{
        .rip = 0x100000,
        .cs = 0x18 | 3,
        .rsp = 0x100000,
        .ss = 0x20 | 3,
    };

    //    auto size = reinterpret_cast<size_t>(&user_text_end) - reinterpret_cast<size_t>(&user_text_start);
//    auto physical_address = TRY(memory_manager.kernel_virtual_to_physical_address(VirtualAddress(&user_text_start)));

    auto page_directory = TRY(memory_manager.create_user_mode_directory());
    process->set_page_directory(page_directory);

    auto stack_physical_address = TRY(memory_manager.kernel_virtual_to_physical_address(stack));
    auto user_stack_virtual = VirtualAddress(0xFF000);
    memory_manager.map_user_page(page_directory, stack_physical_address, user_stack_virtual);
//    memory_manager.map_user_page(page_directory, physical_address, VirtualAddress(0x100000));
    memory_manager.set_user_directory(page_directory);

    Lib::Stack process_stack(user_stack_virtual, Page);
    process_stack.push(initial_stack_frame.ss);
    process_stack.push(initial_stack_frame.rsp);
    process_stack.push(initial_stack_frame.rflags);
    process_stack.push(initial_stack_frame.cs);
    process_stack.push(initial_stack_frame.rip);
    process_stack.push(initial_stack_frame.rax);
    process_stack.push(initial_stack_frame.rbp);
    process_stack.push(initial_stack_frame.rbx);
    process_stack.push(initial_stack_frame.rcx);
    process_stack.push(initial_stack_frame.rdi);
    process_stack.push(initial_stack_frame.rdx);
    process_stack.push(initial_stack_frame.rsi);
    process_stack.push(initial_stack_frame.r8);
    process_stack.push(initial_stack_frame.r9);
    process_stack.push(initial_stack_frame.r10);
    process_stack.push(initial_stack_frame.r11);
    process_stack.push(initial_stack_frame.r12);
    process_stack.push(initial_stack_frame.r13);
    process_stack.push(initial_stack_frame.r14);
    process_stack.push(initial_stack_frame.r15);

    process->set_rsp(process_stack.get_stack_ptr());
    process_list.append(process);
    return {};
}

Process *ProcessManager::next_process() {
    return process_list[m_current_process + 1];
}

Process *ProcessManager::current_process() {
    return process_list[m_current_process];
}
}// namespace Kernel
