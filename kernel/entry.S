.section .text.init

.global _start
_start:
    la sp, STACK_END
.option push
.option norelax
    mv gp, sp
.option pop

    la t0, BSS_START
    la t1, BSS_END
.bss_clear:
    sd zero, (t0)
    addi t0, t0, 8
    bltu t0, t1, .bss_clear

    mv a0, a1
    j kmain
