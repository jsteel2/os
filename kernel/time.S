.section .text

.global time_read
time_read:
    csrr a0, time
    ret
