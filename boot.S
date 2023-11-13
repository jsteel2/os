.section .init

.global _start
_start:
    csrr t0, mhartid
    bnez t0, halt # only allow core 0

    csrw satp, zero # disable MMU

.option push
.option norelax
    la gp, _global_pointer
.option pop

    la sp, _stack_end

    la a0, _bss_start
    la a1, _bss_end
.bss_clear:
    sd zero, (a0)
    addi a0, a0, 8
    bltu a0, a1, .bss_clear # clear BSS

    li t0, 0b11 << 11
    csrw mstatus, t0

    la t1, kmain
    csrw mepc, t1 # set kernel function to jump to

    csrw mie, zero # disable interrupts

    la ra, halt
    mret

halt:
    wfi
    j halt
