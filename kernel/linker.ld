ENTRY(_start);

. = 0x80200000;

SECTIONS {
    PROVIDE(_KERNEL_START = .);

    .text : ALIGN(4K) {
        *(.text.init);
        *(.text);
    }
    .bss : ALIGN(4K) {
        PROVIDE(_BSS_START = .);
        *(.bss);
        *(.sbss);
        PROVIDE(_STACK_START = .);
        . += 4096;
        PROVIDE(_STACK_END = .);
        PROVIDE(_BSS_END = .);
    }
    .rodata : ALIGN(4K) {
        *(.rodata);
    }
    .data : ALIGN(4K) {
        PROVIDE(_GLOBAL_POINTER = . + 0x800);
        *(.data);
        *(.sdata);
    }

    PROVIDE(_KERNEL_END = ALIGN(4K));
}
