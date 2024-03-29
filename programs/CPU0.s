main:                                   # Addr 0x0
	addi	sp, sp, -16
	sw	ra, 12(sp)
	sw	s0, 8(sp)
	addi	s0, sp, 16
	mv	a0, zero
	sw	a0, -12(s0)
	sw	a0, -16(s0)
	j	.LBB0_1
.LBB0_1:                                # Addr 0x20 = 32
	lw	a0, -16(s0)
	addi	a1, zero, 255
	blt	a1, a0, .LBB0_4
	j	.LBB0_2
.LBB0_2:                                # Addr 0x30 = 48
	lui	a0, %hi(1024)
	addi	a0, a0, %lo(1024)
	lw	a1, -16(s0)
	slli	a1, a1, 2
	add	a0, a0, a1
	flw	ft0, 0(a0)
	lui	a0, %hi(2048)
	addi	a0, a0, %lo(2048)
	add	a0, a0, a1
	flw	ft1, 0(a0)
	fadd.s	ft0, ft0, ft1
	lui	a0, %hi(3072)
	addi	a0, a0, %lo(3072)
	add	a0, a0, a1
	fsw	ft0, 0(a0)
	j	.LBB0_3
.LBB0_3:                                # Addr 0x70 == 112
	lw	a0, -16(s0)
	addi	a0, a0, 1
	sw	a0, -16(s0)
	j	.LBB0_1
.LBB0_4:								# Addr 0x80 == 128
	lw	a0, -12(s0)
	lw	s0, 8(sp)
	lw	ra, 12(sp)
	addi	sp, sp, 16
	ret