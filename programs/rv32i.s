foo:

# LUI
lui x1 1048576  # 0

# AUIPC
nop  # 4

# JAL
jal x1 foo  # 8

# JALR
jalr x1 x1 bar  # 12

# BEQ
nop  # 16

# BNE
bne x1 x2 foo  # 20

# BLT
bne x1 x2 bar  # 24

# BGE
nop  # 28

# BLTU
nop  # 32

# BGEU
nop  # 36

bar:

# LB
lb x6 -8(x1)  # 40

# LH
lh x5 8(x1)  # 44

# LW
lw x2 16(x1)  # 48

# LBU
lbu x3 -16(x1)  # 52

# LHU
lhu x4 0(x1)  # 56

# SB
sb x4 0(x1)  # 60

# SH
sh x3 16(x1)  # 64

# SW
sw x2 -16(x1)  # 68

# ADDI
addi x1 x1 -32 # 72

# SLTI
nop  # 76

# SLTIU
nop  # 80

# XORI
nop  # 84

# ORI
nop  # 88

# ANDI
nop  # 92

# SLLI
slli x2 x1 3  # 96

# SRLI
srli x2 x1 4  # 100

# SRAI
srai x2 x1 5  # 104

# ADD
add x3 x1 x2  # 108

# SUB
sub x3 x1 x2  # 112

# SLL
nop  # 116

# SLTU
nop  # 120

# XOR
nop  # 124

# SRL
nop  # 128

# SRA
nop  # 132

# OR
nop  # 136

# AND
nop  # 140

# FENCE -- NOT IMPLEMENTED
nop  # 144

# FENCE.I -- NOT IMPLEMENTED
nop  # 148

# SCALL -- NOT IMPLEMENTED
nop  # 152

# SBREAK -- NOT IMPLEMENTED
nop  # 156

# RDCYCLE
nop  # 160

# RDCYCLEH
nop  # 164

# RDTIME -- NOT IMPLEMENTED
nop  # 168

# RDTIMEH -- NOT IMPLEMENTED
nop  # 172

# RDINSTRET
nop  # 176

# RDINSTRETH
nop  # 180
