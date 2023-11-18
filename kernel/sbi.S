.section .text

.include "macros.S"

.global sbi_call
sbi_call:
# shits its pants when interrupted, FIXME
    addi t6, sp, -13 * 8
    csrw sscratch, t6
 
    save_gp 2, 0
    save_gp 8, 1
    save_gp 9, 2
    .set i, 18
    .rept 9
        save_gp %i, %i - 15
        .set i, i + 1
    .endr

    mv a7, a0
    mv a6, a1
    mv a0, a2
    ecall

    csrr t6, sscratch

    load_gp 2, 0
    load_gp 8, 1
    load_gp 9, 2
    .set i, 18
    .rept 9
        load_gp %i, %i - 15
        .set i, i + 1
    .endr

    ret
