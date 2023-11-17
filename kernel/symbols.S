.section .rodata

.global STACK_START
STACK_START: .dword _STACK_START
.global STACK_END
STACK_END: .dword _STACK_END

.global BSS_START
BSS_START: .dword _BSS_START
.global BSS_END
BSS_END: .dword _BSS_END
