.section .text

.global vmm_enable
vmm_enable:
    li t0, 8 << 60
    srli a0, a0, 12
    or a0, a0, t0
    csrw satp, a0
    sfence.vma
    ret
