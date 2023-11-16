.section .text

.include "macros.S"

.option norvc
.global m_trap_vector
.align 4
m_trap_vector:
    csrrw t6, mscratch, t6

    .set i, 1
    .rept 31
        save_gp %i
        .set i, i + 1
    .endr

    mv t5, t6
    csrr t6, mscratch
    save_gp 31, t5

    csrw mscratch, t5

    csrr t1, satp
    sd t1, 512(t5)

    csrr a0, mepc
    sd a0, 520(t5)
    csrr a1, mtval
    csrr a2, mcause
    csrr a3, mhartid
    csrr a4, mstatus
    csrr a5, mscratch
    la t0, STACK_END
    ld sp, 0(t0)
    call m_trap

    csrw mepc, a0

    csrr t6, mscratch

    .set i, 1
    .rept 31
        load_gp %i
        .set i, i + 1
    .endr

    mret
