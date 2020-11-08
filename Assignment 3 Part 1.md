# CPU0.s
Cpu0.s takes the contents of two floating point arrays and adds them together to form a third array of the same length. Each index in the source arrays gets added to the same index in the other array and stored in the third array. It's essentially just adding two long vectors together.

The c++ code for that would look like
```c++
const int SIZE = 256;
float ARRAY_A[SIZE];
float ARRAY_B[SIZE];
float ARRAY_C[SIZE];

for(int i = 0; i < SIZE; i++){
    ARRAY_C[i] = ARRAY_A[i] + ARRAY_B[i];
}
```
**Note**  
The `cpu0.cpp` file that runs the simulation for Cpu0.s on a single cpu runs this code alongside the simulation and verifies the results against the simulation's final state.

# Instructions
Since there was a lot to do for this assignment, not all of the instructions from the instruction sets have been implemented. All of the macros, pseudoinstructions, assembler relocations, and instructions used in CPU0.s and CPU1.s have been implemented.

The linker that generates a simple binary file out of the assembly does support almost all of the instructions though. So generating binary files that can't be run on the simulation is possible. The simulator will detect if it has encountered an instruction that is doesn't have an implementation for.

The linker also supports the abi names for all registers and can handle a fair number of the pseudoinstructions and macros as well as the %hi and %lo assembler relocation functions.


# Simulation
All of the other requirements for Part 1 should be satisfied. The simulator can execute the binary generated out of `CPU0.s` and reports on the overall cpi of the cpu after it is done.

The `Execute` and `Store` stages in the cpu will both detect memory requests and handle processing the RAM delay by exchanging events with the `Memory` unit. For an example of this sequence played out step by step, check out the `store_test.cpp` file for a unit test that covers this exchange.

Neither unit will attempt to process other instructions while there is an outstanding memory request, so all memory accesses are effectively cpu stalls.

The `Fetch` unit does not currently wait for memory accesses. The assumption was made that the `Fetch` can grab the instructions from some other memory bank that can provide them in a timely manner. This would obviously not hold for larger programs, but could be the case for the small ones we are using.

At the end of the `CPU0.s` simulation, the simulator will print out the total number of clock cycles processed by the simulation as well as the total number of instructions. From those two values the `cpi` of the cpu is calculated and displayed.

