.section .text

# when we have a scheduler, we should check here if we're in a task, and if so use
# the scheduler to lock instead of spinlocking

.global lock_acquire
lock_acquire:
    li t0, 1
1:  amoswap.w.aqrl t0, t0, 0(a0)
    bnez t0, 1b
    ret

.global lock_release
lock_release:
    amoswap.w.aqrl zero, zero, 0(a0)
    ret
