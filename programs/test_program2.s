loop:
    flw f0 0(x1)
    addi x1 x1 -4
    fadd.s f3 f0 f2
    add x0 x0 x0
    add x0 x0 x0
    fsw f3 4(x1)
    bne x1 x2 loop