.section .text.init

.global _start
_start:
    la sp, STACK_END
    call setup_stack

    la t0, BSS_START
    la t1, BSS_END
.bss_clear:
    sd zero, (t0)
    addi t0, t0, 8
    bltu t0, t1, .bss_clear

    j kmain

.global hart_entry
hart_entry:
    mv sp, a1
    call setup_stack

    j kmain_hart

setup_stack:
    addi sp, sp, -632 # trapframe size
    csrw sscratch, sp
    sd a0, 512(sp)

.option push
.option norelax
    la gp, GLOBAL_POINTER
.option pop

    ret
