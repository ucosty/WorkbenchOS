ENTRY(kernel_main)

kernel_vaddr = 0xffffffff80000000;

PHDRS {
  text PT_LOAD FLAGS(0x4 | 0x1);
  user_text PT_LOAD FLAGS(0x4 | 0x1);
  rodata PT_LOAD FLAGS(0x4);
  data PT_LOAD FLAGS(0x4 | 0x2);
  dynamic_segment PT_LOAD FLAGS(0x4 | 0x2);
  dynamic PT_DYNAMIC FLAGS(0x4 | 0x2);
  bss PT_LOAD FLAGS(0x4 | 0x2);
}

SECTIONS {
    . = kernel_vaddr;

    .text ALIGN(4K) : AT(ADDR(.text)) {
        *(.text*)
    } :text

    .user_text ALIGN(4k) : AT(ADDR(.user_text)) {
        user_text_start = .;
       *(.user_text*)
       user_text_end = .;
    } :user_text

    .rodata ALIGN(4k) : AT(ADDR(.rodata)) {
        *(.rodata*)
        start_ctors = .;
        *(.ctors)
        end_ctors = .;
        (.shstrtab*)
    } :rodata

    .data ALIGN(4k) : AT(ADDR(.data)) {
        *(.data*)
    } :data

    .dynamic ALIGN(4K) : AT (ADDR(.dynamic)) {
        *(.dynamic)
    } :dynamic_segment :dynamic

    .bss ALIGN(4K) : AT(ADDR(.bss)) {
        *(.bss*)
        *(COMMON)
    } :bss

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.gnu*)
    }
}