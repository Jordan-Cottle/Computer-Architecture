#include "test.h"

#include "arithmetic_instruction.h"
#include "memory_instruction.h"
#include "control_instructions.h"

using namespace Simulation;

int main()
{
    const int RAM_LOCATION = 124;
    cpu.memory->write(RAM_LOCATION, MFMT(PI));
    cpu.fpRegister.write(2, E);

    // Read/write from ram[124] (offsets in program are set to +/- 4)
    cpu.intRegister.write(1, RAM_LOCATION - 4);

    cpu.loadProgram("fpTest.bin");

    RawInstruction instruction = RawInstruction(cpu.memory->readUint(0));
    Load load = Load(&instruction);

    assert(cpu.fpRegister.read(1) == 0);
    load.execute(&cpu);
    assert(cpu.fpRegister.read(1) == PI);

    instruction = RawInstruction(cpu.memory->readUint(4));
    Add add = Add(&instruction);

    assert(cpu.intRegister.read(1) == RAM_LOCATION - 4);
    add.execute(&cpu);
    assert(cpu.intRegister.read(1) == RAM_LOCATION + 4);

    instruction = RawInstruction(cpu.memory->readUint(8));
    add = Add(&instruction);

    assert(cpu.fpRegister.read(1) == PI);
    assert(cpu.fpRegister.read(2) == E);
    add.execute(&cpu);
    assert(cpu.fpRegister.read(3) - (PI + E) < 0.000001);

    instruction = RawInstruction(cpu.memory->readUint(12));
    Store store = Store(&instruction);

    assert(cpu.memory->readFloat(RAM_LOCATION) == PI);
    store.execute(&cpu);
    assert(cpu.memory->readFloat(RAM_LOCATION) == cpu.fpRegister.read(3));

    instruction = RawInstruction(cpu.memory->readUint(16));
    add = Add(&instruction);

    assert(cpu.intRegister.read(1) == RAM_LOCATION + 4);
    add.execute(&cpu);
    assert(cpu.intRegister.read(1) == RAM_LOCATION);

    instruction = RawInstruction(cpu.memory->readUint(20));
    assert(instruction.data == 0xFEDFF0EF);

    Jump jump = Jump(&instruction);
    assert(jump.offset(&cpu) == -20);
    cpu.programCounter.value = 20;
    jump.execute(&cpu);
    std::cout << cpu.programCounter << "\n";
    assert(cpu.programCounter.value == 0);

    cpu.addPipeline(&fetchUnit);
    cpu.addPipeline(&testPipeline);

    cpu.programCounter.value = 20;
    fetchUnit.stage(&instruction);
    fetchUnit.processInstruction();
    std::cout << cpu.programCounter << "\n";
    assert(cpu.programCounter.value == 0);
    fetchUnit.flush();
    testPipeline.flush();

    instruction = RawInstruction(cpu.memory->readUint(24));
    Blt blt = Blt(&instruction);
    assert(blt.offset(&cpu) == 8);
    fetchUnit.stage(&instruction);
    fetchUnit.processInstruction();
    assert(cpu.programCounter.value == 8);
    std::cout << str(blt.offset(&cpu)) << "\n";

    instruction = RawInstruction(cpu.memory->readUint(32));
    std::cout << instruction.keyword() << "\n";
    assert(instruction.keyword() == "lui");
    Lui lui = Lui(&instruction);
    lui.execute(&cpu);

    instruction = RawInstruction(cpu.memory->readUint(36));
    std::cout << instruction.keyword() << "\n";
    assert(instruction.keyword() == "addi");
    add = Add(&instruction);
    add.execute(&cpu);

    std::cout << str(cpu.intRegister.read(19)) << "\n";
    assert(cpu.intRegister.read(19) == 11437);

    return 0;
}