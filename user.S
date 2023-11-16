.section .text

.include "macros.S"

.global asm_switch_to_user
asm_switch_to_user:
    li t0, (1 << 7) | (1 << 5)
    j switch

.global asm_switch_to_supervisor
asm_switch_to_supervisor:
    li t0, (1 << 11) | (1 << 7) | (1 << 5)
    j switch

switch:
    csrw mscratch, a0

    csrw mstatus, t0

    csrw mepc, a1
    csrw satp, a2

    li t1, 0xaaa
    csrw mie, t1
    la t2, m_trap_vector
    csrw mtvec, t2

    sfence.vma # apparently we dont need to do this every time due to ASID, but i dont know how it works

    mv t6, a0
    .set i, 1
    .rept 31
        load_gp %i, t6
        .set i, i + 1
    .endr

    mret
