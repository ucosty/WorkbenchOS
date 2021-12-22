// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <Types.h>

#define ELF_MAGIC 0x464c457f

#define PT_NULL 0                 // Program header table entry unused
#define PT_LOAD 1                 // Loadable program segment
#define PT_DYNAMIC 2              // Dynamic linking information
#define PT_INTERP 3               // Program interpreter
#define PT_NOTE 4                 // Auxiliary information
#define PT_SHLIB 5                // reserved
#define PT_PHDR 6                 // Entry for header table itself
#define PT_TLS 7                  // Thread-local storage segment
#define PT_NUM 8                  // Number of defined types
#define PT_LOOS 0x60000000        // Start of OS-specific
#define PT_GNU_EH_FRAME 0x6474e550// GCC .eh_frame_hdr segment
#define PT_GNU_STACK 0x6474e551   // Indicates stack executability
#define PT_GNU_RELRO 0x6474e552   // Read-only after relocation
#define PT_LOSUNW 0x6ffffffa
#define PT_SUNWBSS 0x6ffffffa  // Sun Specific segment
#define PT_SUNWSTACK 0x6ffffffb// Stack segment
#define PT_HISUNW 0x6fffffff
#define PT_HIOS 0x6fffffff  // End of OS-specific
#define PT_LOPROC 0x70000000// Start of processor-specific
#define PT_HIPROC 0x7fffffff// End of processor-specific

namespace ELF {
    struct __attribute__((packed)) Elf64_Ehdr {
        uint32_t ei_magic;
        uint8_t ei_class;
        uint8_t ei_data;
        uint8_t ei_version;
        uint8_t ei_osabi;
        uint8_t ei_abiversion;
        uint8_t ei_pad[7];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint64_t e_entry;
        uint64_t e_phoff;
        uint64_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    };

    struct __attribute__((packed)) Elf64_Shdr {
        uint32_t sh_name;
        uint32_t sh_type;
        uint64_t sh_flags;
        uint64_t sh_addr;
        uint64_t sh_offset;
        uint64_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint64_t sh_addralign;
        uint64_t sh_entsize;
    };

    struct __attribute__((packed)) Elf64_Phdr {
        uint32_t p_type;
        uint32_t p_flags;
        uint64_t p_offset;
        uint64_t p_vaddr;
        uint64_t p_paddr;
        uint64_t p_filesz;
        uint64_t p_memsz;
        uint64_t p_align;
    };

    struct Elf64_Rel {
        uint64_t r_offset;
        uint64_t r_info;
    };

    struct Elf64_Rela {
        uint64_t r_offset;
        uint64_t r_info;
        int64_t r_addend;
    };
}// namespace ELF
