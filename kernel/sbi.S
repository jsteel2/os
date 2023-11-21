.section .text

.include "macros.S"

.global sbi_call
sbi_call:
    csrr t6, sscratch
    addi t6, t6, 520

    save_gp 2, 0
    save_gp 3, 1
    save_gp 4, 2
    save_gp 8, 3
    save_gp 9, 4
    .set i, 18
    .rept 9
        save_gp %i, %i - 13
        .set i, i + 1
    .endr

    mv a7, a0
    mv a6, a1
    mv a0, a2
    mv a1, a3
    mv a2, a4
    ecall

    csrr t6, sscratch
    addi t6, t6, 520

    load_gp 2, 0
    load_gp 3, 1
    load_gp 4, 2
    load_gp 8, 3
    load_gp 9, 4
    .set i, 18
    .rept 9
        load_gp %i, %i - 13
        .set i, i + 1
    .endr

    ret
