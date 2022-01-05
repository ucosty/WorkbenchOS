// Application Processor
// Boot Trampoline
.org 0x8000
.code16
.extern ap_main
.global ap_trampoline
ap_trampoline:
    cli
    cld
    ljmp $0, $0x8040

.align 16
gdt_table:
    .long 0, 0
    .long 0x0000FFFF, 0x00CF9A00 // 32-bit code
    .long 0x0000FFFF, 0x008F9200 // 32-bit data
    .long 0x00000000, 0x00209800 // 64-bit code
    .long 0x00000000, 0x00009000 // 64-bit data
    
gdt_pointer:
    .word gdt_pointer - gdt_table - 1
    .long 0x8010

.align 64
init_pmode:
    xorw %ax, %ax
    movw %ax, %ds
    lgdtl 0x8038

    // Enable protected mode
    movl %cr0, %eax
    orl $1, %eax
    movl %eax, %cr0

    // Jump into protected mode
    ljmp $0x08, $0x8060

.align 32
.code32
init_longmode:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %ss

    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    // Load CR3 with the physical address of the PML4
    mov (0x8ff0), %eax
    mov %eax, %cr3

    // Enable long mode by setting the EFER.LME flag in MSR 0xC0000080
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr

    // Enabling paging now will also enable IA32_EFER.LMA
    mov %cr0, %ecx
    or $0x80000000, %ecx
    mov %ecx, %cr0

    // Jump into long mode
    ljmpl $0x18, $0x8500

.align 2048
.code64
longmode:
    mov (0x8fe0), %rax
    mov (0x8fd0), %rsp
    jmpq *%rax

.global end_ap_trampoline
end_ap_trampoline:
