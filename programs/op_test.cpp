#include "test.h"
#include "arithmetic_instruction.h"
#include "memory_instruction.h"

int main()
{
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

    testCpu->loadProgram("op_test.bin");

    uint32_t memoryAddress = 200;
    int value = 42;

    int pc = 0;
    RawInstruction instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "addi");
    Add add = Add(&instruction);
    add.execute(testCpu);

    assert(testCpu->intRegister.read(1) == (int)memoryAddress);

    pc = 4;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "addi");
    add = Add(&instruction);
    add.execute(testCpu);

    assert(testCpu->intRegister.read(2) == value);

    pc = 8;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "sb");
    Store store = Store(&instruction);
    store.execute(testCpu);

    assert(testCpu->intRegister.read(1) == (int)memoryAddress);
    assert(testCpu->intRegister.read(2) == value);
    assert(testRam->data[memoryAddress] == value);

    pc = 12;
    instruction = RawInstruction(testRam->readUint(pc));
    assert(instruction.keyword() == "lb");
    Load load = Load(&instruction);
    load.execute(testCpu);

    assert(testCpu->intRegister.read(1) == (int)memoryAddress);
    assert(testCpu->intRegister.read(2) == value);
    assert(testCpu->intRegister.read(3) == value);
    assert(testRam->data[memoryAddress] == value);
}