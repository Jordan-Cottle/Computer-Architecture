# Assignment 3 Part 1

Assignment 3 part 1 involved adding RAM to store instructions and data. The assembly program `CPU0.s` is loaded into RAM and executed by a single core CPU. The final CPI of the cpu is printed out after the memory state is verified to match the output of the equivalent c++ logic to `CPU0.s`.

## CPU0.s
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

## Instructions
Since there was a lot to do for this assignment, not all of the instructions from the instruction sets have been implemented. All of the macros, pseudoinstructions, assembler relocations, and instructions used in CPU0.s and CPU1.s have been implemented.

The linker that generates a simple binary file out of the assembly does support almost all of the instructions though. So generating binary files that can't be run on the simulation is possible. The simulator will detect if it has encountered an instruction that is doesn't have an implementation for.

The linker also supports the abi names for all registers and can handle a fair number of the pseudoinstructions and macros as well as the %hi and %lo assembler relocation functions.


## Simulation
All of the other requirements for Part 1 should be satisfied. The simulator can execute the binary generated out of `CPU0.s` and reports on the overall cpi of the cpu after it is done.

The `Execute` and `Store` stages in the cpu will both detect memory requests and handle processing the RAM delay by exchanging events with the `Memory` unit. For an example of this sequence played out step by step, check out the `store_test.cpp` file for a unit test that covers this exchange.

Neither unit will attempt to process other instructions while there is an outstanding memory request, so all memory accesses are effectively cpu stalls.

The `Fetch` unit does not currently wait for memory accesses. The assumption was made that the `Fetch` can grab the instructions from some other memory bank that can provide them in a timely manner. This would obviously not hold for larger programs, but could be the case for the small ones we are using.

At the end of the `CPU0.s` simulation, the simulator will print out the total number of clock cycles processed by the simulation as well as the total number of instructions. From those two values the `cpi` of the cpu is calculated and displayed.

# Part 2

Assignment 3 part 2 adds a memory bus and an additional cpu core to the simulation. The second cpu executes the `CPU1.s` assembly program and shares data memory with the first cpu.

## Memory Bus
The memory bus handles requests to the main memory and ensures that only one cpu is accessing the memory at a time.

Currently the memory bus only processes one request at a time, even if the requested memory addresses are on separate data ports that could be read simultaneously. These requests take 5 simulation cycles each, so both cpus could make independent requests that get processed before the next cpu cycle making them effectively handled in parallel.

## Second Cpu
The second cpu has exactly the same structure and configuration as the first. The only difference is that it's stack pointer is set differently and the program is loaded into a different starting memory location.

## Simulation
Since the memory bus and the ram both have latencies, the CPI for this simulation has increased a bit. The two cpus have almost exactly the same cpi though. Once the initial memory request for the same data is sorted out, one of the cpus is delayed by a couple cycles. This sets the cpus up to not make the same request to memory again for the rest of the program since they never issue a memory accessing instruction at the same time again. As a consequence of this initial delay the cpu that gets its memory request handled first finished running the program slightly sooner than the other one. The number of instructions execute remains the same for both, but one of them will have taken 2 cycles longer than the other.

Just like with part 1, the behavior of `CPU1.s` is run independently alongside the simulation to generate an output array that is verified against the state of the shared cpu memory. If any of the memory addresses don't match the expected output, the simulation will crash before giving its `CPI` report.
