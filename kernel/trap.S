.section .text

.include "macros.S"

.global enable_interrupts
enable_interrupts:
    li t0, 0xaaa
    csrw sie, t0
    la t1, trap_vector
    csrw stvec, t1
    csrsi sstatus, 1 << 1
    ret

.option norvc
.global trap_vector
.align 4
trap_vector:
    csrrw t6, sscratch, t6

    .set i, 1
    .rept 30
        save_gp %i, %i
        .set i, i + 1
    .endr

    mv t5, t6
    csrrw t6, sscratch, t6
    save_gp 31, 31, t5

    csrr a0, sepc
    csrr a1, stval
    csrr a2, scause
    mv a3, t5
    mv sp, t5
    call trap

    csrw sepc, a0

    mv t6, sp
    .set i, 1
    .rept 31
        load_gp %i, %i
        .set i, i + 1
    .endr

    sret
