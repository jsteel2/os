.section .text

.global asm_virt_enable
asm_virt_enable:
    csrw satp, a0

    li t0, (1 << 8) | (1 << 5)
    csrw sstatus, t0

    csrw sepc, ra

    li t2, (1 << 1) | (1 << 5) | (1 << 9)
    csrw mideleg, t2

    csrw sie, t2

    la t3, asm_trap_vector
    csrw stvec, t3

    li t4, 0
    csrw pmpaddr0, t4

    li t5, 7 | (1 << 3)
    csrw pmpcfg0, t5

    sfence.vma
    sret
