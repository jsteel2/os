.section .text

.global asm_virt_enable
asm_virt_enable:
    csrw satp, a0

    csrw mscratch, a1
    csrw sscratch, a1

    li t0, (1 << 11) | (1 << 7) | (1 << 5)
    csrw mstatus, t0

    la t1, m_trap_vector
    csrw mtvec, t1

    csrw mepc, ra

    li t2, 0xaaa
    csrw mie, t2

    li t3, 0
    csrw pmpaddr0, t3

    li t4, 7 | (1 << 3)
    csrw pmpcfg0, t4

    sfence.vma
    mret
