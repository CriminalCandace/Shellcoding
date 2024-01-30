	.globl	_start
	
	.text
_start:
	li	a7, 64
	li	a0, 1
	#la	a1, msg
0:
	auipc	a1, %pcrel_hi(msg)
	addi	a1, a1, %pcrel_lo(0b)
	li	a2, 26
	ecall
	li	a7, 94
	li	a0, 0
	ecall

	.data
msg:
	.string "Hello, shellcoding world!\n"
