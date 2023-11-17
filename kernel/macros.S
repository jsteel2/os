.altmacro
.set NUM_GP_REGS, 32
.set REG_SIZE, 8

.macro save_gp i, where, basereg=t6
	sd	x\i, ((\where)*REG_SIZE)(\basereg)
.endm
.macro load_gp i, where, basereg=t6
	ld	x\i, ((\where)*REG_SIZE)(\basereg)
.endm

