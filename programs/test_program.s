loop:
    flw f0 0(x1)
    add x0 x0 x0
    fadd.s f3 f0 f2
    add x0 x0 x0
    add x0 x0 x0
    fsw f3 0(x1)
    addi x1 x1 -4
    bne x1 x2 loop