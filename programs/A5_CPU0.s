main:                                   # 0x0 : 0
	addi	sp, sp, -32
	sw	ra, 28(sp)
	sw	s0, 24(sp)
	addi	s0, sp, 32
	mv	a0, zero
	sw	a0, -12(s0)
	sh	a0, -14(s0)
	j	.LBB0_1
.LBB0_1:                                # 0x20 : 32
	lhu	a0, -14(s0)
	addi	a1, zero, 99
	blt	a1, a0, .LBB0_16
	j	.LBB0_2
.LBB0_2:                                # 0x30 : 48
	mv	a0, zero
	sh	a0, -16(s0)
	j	.LBB0_3
.LBB0_3:                                # 0x3C : 60
	lhu	a0, -16(s0)
	addi	a1, zero, 99
	blt	a1, a0, .LBB0_14
	j	.LBB0_4
.LBB0_4:                                # 0x4C : 76
	mv	a0, zero
	sh	a0, -22(s0)
	sh	a0, -18(s0)
	j	.LBB0_5
.LBB0_5:                                # 0x5C : 92
	lhu	a0, -18(s0)
	addi	a1, zero, 2
	blt	a1, a0, .LBB0_12
	j	.LBB0_6
.LBB0_6:                                # 0x6C : 108
	mv	a0, zero
	sh	a0, -20(s0)
	j	.LBB0_7
.LBB0_7:                                # 0x78 : 120
	lhu	a0, -20(s0)
	addi	a1, zero, 2
	blt	a1, a0, .LBB0_10
	j	.LBB0_8
.LBB0_8:                                # 0x88 : 136
	lui	a0, %hi(1024)
	addi	a0, a0, %lo(1024)
	lhu	a1, -14(s0)
	lhu	a2, -18(s0)
	add	a1, a1, a2
	addi	a3, zero, 102
	mul	a1, a1, a3
	lhu	a3, -16(s0)
	lhu	a4, -20(s0)
	add	a3, a3, a4
	add	a1, a1, a3
	add	a0, a0, a1
	lbu	a0, 0(a0)
	lui	a1, %hi(11428)
	addi	a1, a1, %lo(11428)
	addi	a3, zero, 3
	mul	a2, a2, a3
	add	a2, a2, a4
	add	a1, a1, a2
	lbu	a1, 0(a1)
	mul	a0, a0, a1
	lh	a1, -22(s0)
	add	a0, a1, a0
	sh	a0, -22(s0)
	j	.LBB0_9
.LBB0_9:                                # 0xEC : 236
	lh	a0, -20(s0)
	addi	a0, a0, 1
	sh	a0, -20(s0)
	j	.LBB0_7
.LBB0_10:                               # 0xFC : 252
	j	.LBB0_11
.LBB0_11:                               # 0x100 : 256
	lh	a0, -18(s0)
	addi	a0, a0, 1
	sh	a0, -18(s0)
	j	.LBB0_5
.LBB0_12:                               # 0x110 : 272
	lh	a0, -22(s0)
	lui	a1, %hi(11437)
	addi	a1, a1, %lo(11437)
	lhu	a2, -14(s0)
	addi	a3, zero, 100
	mul	a2, a2, a3
	lhu	a3, -16(s0)
	add	a2, a2, a3
	slli	a2, a2, 1
	add	a1, a1, a2
	sh	a0, 0(a1)
	j	.LBB0_13
.LBB0_13:                               # 0x140 : 320
	lh	a0, -16(s0)
	addi	a0, a0, 1
	sh	a0, -16(s0)
	j	.LBB0_3
.LBB0_14:                               # 0x150 : 336
	j	.LBB0_15
.LBB0_15:                               # 0x154 : 340
	lh	a0, -14(s0)
	addi	a0, a0, 1
	sh	a0, -14(s0)
	j	.LBB0_1
.LBB0_16:								# 0x164 : 356
	lw	a0, -12(s0)
	lw	s0, 24(sp)
	lw	ra, 28(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end0:							# 0x178 : 376