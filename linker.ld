ENTRY(_start);

ram = 0x80000000;
ram_size = 32M;
. = ram;
 
SECTIONS {
	.text : ALIGN(4K) {
        PROVIDE(_text_start = .);
		*(.init);
		*(.text);
        PROVIDE(_text_end = .);
	}
	.bss : ALIGN(4K) {
		PROVIDE(_bss_start = .);
		*(.bss);
		. += 4096;
		PROVIDE(_stack_top = .);
		. += 4096;
		PROVIDE(_global_pointer = .);
		PROVIDE(_bss_end = .);
	}
	.rodata : ALIGN(4K) {
        PROVIDE(_rodata_start = .);
		*(.rodata);
        PROVIDE(_rodata_end = .);
	}
	.data : ALIGN(4K) {
        PROVIDE(_data_start = .);
		*(.data);
        PROVIDE(_data_end = .);
	}

    . = ALIGN(4K);

    PROVIDE(_memory_start = ram);
    PROVIDE(_memory_end = ram + ram_size);
    PROVIDE(_heap_start = .);
    PROVIDE(_heap_size = _memory_end - _heap_start);
}
