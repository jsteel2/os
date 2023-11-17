.section .text

.include "macros.S"

.option norvc
.global trap_vector
.align 4
trap_vector:
    csrw sscratch, t6
    la t6, STACK_END - 536

    .set i, 1
    .rept 30
        save_gp %i, %i
        .set i, i + 1
    .endr

    csrr t1, satp
    sd t1, 512(t6)

    csrr a0, sepc
    csrr a1, stval
    csrr a2, scause
    #csrr a3, mhartid HOW to get hart id?
    csrr a4, sstatus
    mv a5, t6
    mv sp, t6
    call trap

    csrw sepc, a0

    .set i, 1
    .rept 30
        load_gp %i, %i, sp
        .set i, i + 1
    .endr

    csrr t6, sscratch

    sret
