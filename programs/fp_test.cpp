#include "test.h"

#include "arithmetic_instruction.h"
#include "memory_instruction.h"
#include "control_instructions.h"

using namespace Simulation;

int main()
{
    const int RAM_LOCATION = 124;
    cpu.memory.write(RAM_LOCATION, PI);
    cpu.fpRegister.write(2, E);

    // Read/write from ram[124] (offsets in program are set to +/- 4)
    cpu.intRegister.write(1, RAM_LOCATION - 4);

    cpu.loadProgram("fpTest.bin");

    RawInstruction instruction = RawInstruction(cpu.memory.read<uint32_t>(0));
    Load load = Load(&instruction);

    assert(cpu.fpRegister.read(1) == 0);
    load.execute(&cpu);
    assert(cpu.fpRegister.read(1) == PI);

    instruction = RawInstruction(cpu.memory.read<uint32_t>(4));
    Add add = Add(&instruction);

    assert(cpu.intRegister.read(1) == RAM_LOCATION - 4);
    add.execute(&cpu);
    assert(cpu.intRegister.read(1) == RAM_LOCATION + 4);

    instruction = RawInstruction(cpu.memory.read<uint32_t>(8));
    add = Add(&instruction);

    assert(cpu.fpRegister.read(1) == PI);
    assert(cpu.fpRegister.read(2) == E);
    add.execute(&cpu);
    assert(cpu.fpRegister.read(3) - (PI + E) < 0.000001);

    instruction = RawInstruction(cpu.memory.read<uint32_t>(12));
    Store store = Store(&instruction);

    assert(cpu.memory.read<float>(RAM_LOCATION) == PI);
    store.execute(&cpu);
    assert(cpu.memory.read<float>(RAM_LOCATION) == cpu.fpRegister.read(3));

    instruction = RawInstruction(cpu.memory.read<uint32_t>(16));
    add = Add(&instruction);

    assert(cpu.intRegister.read(1) == RAM_LOCATION + 4);
    add.execute(&cpu);
    assert(cpu.intRegister.read(1) == RAM_LOCATION);

    instruction = RawInstruction(cpu.memory.read<uint32_t>(20));
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
    fetchUnit.tick();
    std::cout << cpu.programCounter << "\n";
    assert(cpu.programCounter.value == 0);
    fetchUnit.flush();
    testPipeline.flush();

    instruction = RawInstruction(cpu.memory.read<uint32_t>(24));
    Blt blt = Blt(&instruction);
    assert(blt.offset(&cpu) == 8);
    fetchUnit.stage(&instruction);
    fetchUnit.tick();
    assert(cpu.programCounter.value == 8);
    std::cout << str(blt.offset(&cpu)) << "\n";

    return 0;
}