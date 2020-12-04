#include "test.h"

#include "arithmetic_instruction.h"
#include "control_instructions.h"
#include "memory_instruction.h"

constexpr int FOO = 0;
constexpr int BAR = 40;

int main()
{
    int pc = 0;
    Memory *testRam = new Memory(0, MEMORY_SIZE);
    Cpu *testCpu = new Cpu(testRam);

    Fetch *testFetch = new Fetch(testCpu);
    Decode *testDecode = new Decode(testCpu);
    Execute *testExecute = new Execute(testCpu);
    StorePipeline *testStore = new StorePipeline(testCpu);
    testCpu->addPipeline(testFetch);
    testCpu->addPipeline(testDecode);
    testCpu->addPipeline(testExecute);
    testCpu->addPipeline(testStore);

    testCpu->loadProgram("rv32i.bin");

    // LUI  # 0
    pc = 0;
    RawInstruction instruction = RawInstruction(testRam->readUint(pc));
    Lui lui = Lui(&instruction);
    lui.execute(testCpu);

    assert(testCpu->intRegister.read(1) == 1048576);

    // AUIPC  # 4
    pc = 4;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    Add nop = Add(&instruction);

    // JAL  # 8
    pc = 8;
    testCpu->programCounter.value = pc;
    testCpu->jumpedFrom = 8; // Mock Fetch unit behavior
    instruction = RawInstruction(testRam->readUint(pc));
    Jump jal = Jump(&instruction);
    jal.execute(testCpu);

    assert(jal.offset(testCpu) == FOO - pc);
    assert(testCpu->intRegister.read(1) == pc + 4);
    assert(testCpu->programCounter.value == FOO);

    // JALR  # 12
    pc = 12;
    testCpu->programCounter.value = pc;
    testCpu->jumpedFrom = pc; // Mock Fetch unit behavior
    instruction = RawInstruction(testRam->readUint(pc));
    Jalr jalr = Jalr(&instruction);
    int regOffset = 0;
    testCpu->intRegister.write(1, regOffset);
    assert(jalr.offset(testCpu) == BAR - pc + regOffset);
    regOffset = 4;
    testCpu->intRegister.write(1, regOffset);
    assert(jalr.offset(testCpu) == BAR - pc + regOffset);
    regOffset = -4;
    testCpu->intRegister.write(1, regOffset);
    assert(jalr.offset(testCpu) == BAR - pc + regOffset);
    jalr.execute(testCpu);

    assert(testCpu->programCounter.value == BAR + regOffset);

    // BEQ  # 16
    pc = 12;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // BNE  # 20
    pc = 20;
    testCpu->programCounter.value = pc;
    testCpu->jumpedFrom = pc; // Mock Fetch unit behavior
    instruction = RawInstruction(testRam->readUint(pc));
    Bne bne = Bne(&instruction);
    assert(bne.offset(testCpu) == FOO - pc);

    testCpu->intRegister.write(1, 10);
    testCpu->intRegister.write(2, 11);
    bne.execute(testCpu);
    assert(testCpu->programCounter.value == FOO);

    // Test speculated branch actually fails
    testCpu->intRegister.write(2, 10);
    testCpu->branchSpeculated = true;
    bne.execute(testCpu);
    assert(testCpu->programCounter.value == pc + 4);

    // BLT  # 24
    pc = 24;
    testCpu->programCounter.value = pc;
    testCpu->jumpedFrom = pc; // Mock Fetch unit behavior
    instruction = RawInstruction(testRam->readUint(pc));
    Blt blt = Blt(&instruction);
    assert(blt.offset(testCpu) == BAR - pc);

    // reset predictor so branch triggers in execute
    testCpu->branchSpeculated = false;
    testCpu->intRegister.write(1, 10);
    testCpu->intRegister.write(2, 11);
    blt.execute(testCpu);
    assert(testCpu->programCounter.value == BAR);

    // Test speculated branch actually fails
    testCpu->intRegister.write(2, 10);
    testCpu->branchSpeculated = true;
    bne.execute(testCpu);
    assert(testCpu->programCounter.value == pc + 4);

    // BGE
    pc = 28;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // BLTU
    pc = 32;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // BGEU
    pc = 36;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // LB
    pc = 40;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "lb");
    Load load = Load(&instruction);

    uint32_t memoryLocation = 800;
    testCpu->intRegister.write(1, memoryLocation + 8);

    assert(load.memoryOffset == -8);
    assert(load.memoryAddress(testCpu) == memoryLocation);
    assert(!load.isFp);

    uint32_t data = 0x12345678;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    int value = testCpu->intRegister.read(6);
    assert(value == 0x78);

    data = 0x123456A8;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(6);
    assert(value == (int)0xFFFFFFFA8);

    // LH
    pc = 44;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "lh");
    load = Load(&instruction);

    memoryLocation = 700;
    testCpu->intRegister.write(1, memoryLocation - 8);

    assert(load.memoryOffset == 8);
    assert(load.memoryAddress(testCpu) == memoryLocation);
    assert(!load.isFp);

    data = 0x12345678;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(5);
    assert(value == 0x5678);

    data = 0x1234A678;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(5);
    assert(value == (int)0xFFFFA678);

    // LW
    pc = 48;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    load = Load(&instruction);

    memoryLocation = 400;
    data = 42;
    testRam->write(memoryLocation, MFMT(data));
    testCpu->intRegister.write(1, memoryLocation - 16); // load has offset of 16 set as immediate
    load.execute(testCpu);
    assert(testCpu->intRegister.read(2) == 42);

    // LBU
    pc = 52;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "lbu");
    load = Load(&instruction);

    memoryLocation = 500;
    testCpu->intRegister.write(1, memoryLocation + 16);

    assert(load.memoryOffset == -16);
    assert(load.memoryAddress(testCpu) == memoryLocation);
    assert(!load.isFp);

    data = 0x12345678;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(3);
    assert(value == 0x78);

    data = 0x123456A8;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(3);
    assert(value == (int)0xA8);

    // LHU
    pc = 56;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "lhu");
    load = Load(&instruction);

    memoryLocation = 400;
    testCpu->intRegister.write(1, memoryLocation);

    assert(load.memoryOffset == 0);
    assert(load.memoryAddress(testCpu) == memoryLocation);
    assert(!load.isFp);

    data = 0x12345678;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(4);
    assert(value == 0x5678);

    data = 0x1234A678;
    testCpu->memory->write(memoryLocation, MFMT(data));
    load.execute(testCpu);
    value = testCpu->intRegister.read(4);
    assert(value == (int)0xA678);

    // SB
    pc = 60;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "sb");
    Store store = Store(&instruction);

    memoryLocation = 300;
    testCpu->intRegister.write(1, memoryLocation);
    testCpu->intRegister.write(4, 0x12345678);

    data = 0x87654321;
    testRam->write(memoryLocation, MFMT(data));

    store.execute(testCpu);
    assert(testRam->readUint(memoryLocation) == 0x87654378); // Only first two hex digits written
    assert(testRam->data[memoryLocation] == 0x78);           // Only first two hex digits written

    // SH
    pc = 64;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "sh");
    store = Store(&instruction);

    memoryLocation = 400;
    testCpu->intRegister.write(1, memoryLocation - 16);
    testCpu->intRegister.write(3, 0x12345678);

    data = 0x87654321;
    testRam->write(memoryLocation, MFMT(data));

    store.execute(testCpu);
    assert(testRam->readUint(memoryLocation) == 0x87655678); // Only first 4 hex digits written
    assert(testRam->data[memoryLocation] == 0x78);           // Only first 4 hex digits written
    assert(testRam->data[memoryLocation + 1] == 0x56);       // Only first 4 hex digits written

    // SW
    pc = 68;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    store = Store(&instruction);

    memoryLocation = 500;
    testCpu->intRegister.write(1, memoryLocation + 16); // store has -16 offset set
    testCpu->intRegister.write(2, 300);

    store.execute(testCpu);
    assert(testRam->readInt(memoryLocation) == 300);

    // ADDI
    pc = 72;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    Add addi = Add(&instruction);

    testCpu->intRegister.write(1, 16);
    addi.execute(testCpu);
    assert(testCpu->intRegister.read(1) == -16);

    // SLTI
    pc = 76;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SLTIU
    pc = 80;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // XORI
    pc = 84;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // ORI
    pc = 88;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // ANDI
    pc = 92;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SLLI
    pc = 96;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "slli");
    Shift shift = Shift(&instruction);

    testCpu->intRegister.write(1, 3);
    shift.execute(testCpu);
    assert(testCpu->intRegister.read(2) == 3 << 3);

    // SRLI
    pc = 100;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "srli");
    shift = Shift(&instruction);

    testCpu->intRegister.write(1, 0b10000);
    shift.execute(testCpu);
    assert(testCpu->intRegister.read(2) == 1);

    // SRAI
    pc = 104;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "srai");
    shift = Shift(&instruction);

    testCpu->intRegister.write(1, -32);
    shift.execute(testCpu);
    assert(testCpu->intRegister.read(2) == -1);

    testCpu->intRegister.write(1, 0b00100000);
    shift.execute(testCpu);
    assert(testCpu->intRegister.read(2) == 1);

    // ADD
    pc = 108;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    Add add = Add(&instruction);

    testCpu->intRegister.write(1, -10);
    testCpu->intRegister.write(2, 24);
    add.execute(testCpu);
    assert(testCpu->intRegister.read(3) == 14);

    // SUB
    pc = 112;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    Sub sub = Sub(&instruction);

    testCpu->intRegister.write(1, 32);
    testCpu->intRegister.write(2, 64);
    sub.execute(testCpu);
    assert(testCpu->intRegister.read(3) == -32);

    // SLL
    pc = 116;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SLTU
    pc = 120;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // XOR
    pc = 124;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SRL
    pc = 128;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SRA
    pc = 132;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // OR
    pc = 136;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // AND
    pc = 140;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // FENCE -- NOT IMPLEMENTED
    pc = 144;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // FENCE.I -- NOT IMPLEMENTED
    pc = 148;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SCALL -- NOT IMPLEMENTED
    pc = 152;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // SBREAK -- NOT IMPLEMENTED
    pc = 156;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // RDCYCLE
    pc = 160;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // RDCYCLEH
    pc = 164;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // RDTIME -- NOT IMPLEMENTED
    pc = 168;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // RDTIMEH -- NOT IMPLEMENTED
    pc = 172;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // RDINSTRET
    pc = 176;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);

    // RDINSTRETH
    pc = 180;
    testCpu->programCounter.value = pc;
    instruction = RawInstruction(testRam->readUint(pc));
    nop = Add(&instruction);
}