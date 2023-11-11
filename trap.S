.section .text

.global asm_trap_vector
asm_trap_vector:
    li t0, 0x10000000
    li t1, 'X'
    sb t1, (t0)
.l:
    j .l
