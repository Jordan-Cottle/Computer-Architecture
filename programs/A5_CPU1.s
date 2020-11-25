main:                                   # 0x200
	addi	sp, sp, -32
	sw	ra, 28(sp)
	sw	s0, 24(sp)
	addi	s0, sp, 32
	mv	a0, zero
	sw	a0, -12(s0)
	sh	a0, -14(s0)
	j	.LBB0_1
.LBB0_1:                                # 0x220
	lhu	a0, -14(s0)
	addi	a1, zero, 99
	blt	a1, a0, .LBB0_11
	j	.LBB0_2
.LBB0_2:                                # 0x230
	mv	a0, zero
	sh	a0, -16(s0)
	j	.LBB0_3
.LBB0_3:                                # 0x23C
	lhu	a0, -16(s0)
	addi	a1, zero, 99
	blt	a1, a0, .LBB0_9
	j	.LBB0_4
.LBB0_4:                                # 0x24C
	j	.LBB0_5
.LBB0_5:                                # 0x250
	lui	a0, %hi(11437)
	addi	a0, a0, %lo(11437)
	lhu	a1, -14(s0)
	addi	a2, zero, 100
	mul	a1, a1, a2
	lhu	a2, -16(s0)
	add	a1, a1, a2
	slli	a1, a1, 1
	add	a0, a0, a1
	lh	a0, 0(a0)
	addi	a1, zero, -1
	bne	a0, a1, .LBB0_7
	j	.LBB0_6
.LBB0_6:                                # 0x284
	j	.LBB0_5
.LBB0_7:                                # 0x288
	lui	a0, %hi(11437)
	addi	a0, a0, %lo(11437)
	lhu	a1, -14(s0)
	addi	a2, zero, 100
	mul	a1, a1, a2
	lhu	a3, -16(s0)
	add	a1, a1, a3
	slli	a1, a1, 1
	add	a0, a0, a1
	lh	a0, 0(a0)
	sh	a0, -20(s0)
	lh	a0, -20(s0)
	srli	a0, a0, 4
	lui	a1, %hi(31437)
	addi	a1, a1, %lo(31437)
	lhu	a3, -14(s0)
	mul	a2, a3, a2
	lhu	a3, -16(s0)
	add	a2, a2, a3
	add	a1, a1, a2
	sb	a0, 0(a1)
	j	.LBB0_8
.LBB0_8:                                # 0x2E0
	lh	a0, -16(s0)
	addi	a0, a0, 1
	sh	a0, -16(s0)
	j	.LBB0_3
.LBB0_9:                                # 0x2F0
	j	.LBB0_10
.LBB0_10:                               # 0x2F4
	lh	a0, -14(s0)
	addi	a0, a0, 1
	sh	a0, -14(s0)
	j	.LBB0_1
.LBB0_11:								# 0x304
	lw	a0, -12(s0)
	lw	s0, 24(sp)
	lw	ra, 28(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end0:							# 0x318