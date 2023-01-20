ENTRY(_start)

kernel_base = 0xffffffff80000000;

PHDRS {
  text PT_LOAD FLAGS(0x4 | 0x1);
  rodata PT_LOAD FLAGS(0x4);
  data PT_LOAD FLAGS(0x4 | 0x2);
  init_array PT_LOAD FLAGS(0x4 | 0x2);
  dynamic_segment PT_LOAD FLAGS(0x4 | 0x2);
  dynamic PT_DYNAMIC FLAGS(0x4 | 0x2);
  bss PT_LOAD FLAGS(0x4 | 0x2);
}

SECTIONS {
    . = kernel_base;

    .text ALIGN(4K) : AT(ADDR(.text) - kernel_base) {
        *(.text*)
    } :text

    .rodata ALIGN(4k) : AT(ADDR(.rodata) - kernel_base) {
        *(.rodata*)

        *(SHT_STRTAB)
        *(SHT_PROGBITS)
    } :rodata

    .data ALIGN(4k) : AT(ADDR(.data) - kernel_base) {
        *(.data*)
    } :data

    .init_array : AT(ADDR(.init_array) - kernel_base) {
    } :init_array

    .dynamic ALIGN(4K) : AT (ADDR(.dynamic) - kernel_base) {
        *(.dynamic)
    } :dynamic_segment :dynamic

    .bss ALIGN(4K) : AT(ADDR(.bss) - kernel_base) {
        *(.bss*)
        *(COMMON)
    } :bss

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.gnu*)
    }
}