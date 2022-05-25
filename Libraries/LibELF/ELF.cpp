// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <LibELF/ELF.h>

void load_elf_sections(void *bytes, ELF::Elf64_Ehdr * elf_header) {
    auto program_headers = (ELF::Elf64_Phdr *) ((char *) bytes + elf_header->e_phoff);

    for (int i = 0; i < elf_header->e_phnum; i++) {
        auto program_header = &program_headers[i];
        if (program_header->p_type != PT_LOAD) {
            continue;
        }

//        auto load_pages = bytes_to_pages(program_header->p_memsz);
//        auto physical_address = kernel_virtual_to_physical(kernel_physical_base, program_header->p_vaddr);
//        auto load_address = (void *) ((char *) bytes + program_header->p_offset);
//        memset((char *) physical_address, 0, load_pages * 0x1000);
//        memcpy((void *) physical_address, load_address, program_header->p_filesz);
    }
}
