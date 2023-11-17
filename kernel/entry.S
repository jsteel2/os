.section .text.init

.global _start
_start:
    la sp, STACK_END
.option push
.option norelax
    mv gp, sp
.option pop

    la a0, BSS_START
    la a1, BSS_END
.bss_clear:
    sd zero, (a0)
    addi a0, a0, 8
    bltu a0, a1, .bss_clear

    j kmain

halt:
    li a7, 0x48534D
    li a6, 3
    li a0, 0
    ecall
    j halt
