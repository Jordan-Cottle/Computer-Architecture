foo:
    flw f1 4(x1)
    addi x1 x1 8
    fadd.s f3 f1 f2
    fsw f3 -4(x1)
    addi x1 x1 -4
    jal foo
    blt x0 x1 yeet
    nop
yeet:
    nop
    nop
