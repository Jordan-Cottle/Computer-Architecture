# Mesi Protocol

Unfortunately I was unable to get a completely functional simulation with the mesi cache coherence protocol set up. 

The state transitions are all in place and the `cache_test.cpp` file runs through all of the transitions as well as an isolated simulation of memory requests. To see the the results run `make cache_test`.

The main problem with running the whole simulation was with the memory requests. After setting up the caches with write back mesi instead of the default write-through I had from the last assignment I realized I needed to be able to cancel a memory request that was in progress. The caches complain much more aggressively about not having the right memory addresses loaded, and the memory requests from the fetch unit that were in flight during a pipeline flush were causing a ton of problems.

I started off trying to just handle specific edge cases, but the fetch unit ended up becoming a horrible state machine to try and keep track of lots of little bits of information about a request that didn't have a solid form or representation in the simulation. I decided to cut my losses and refactor the entire memory request mechanism to support an actual `MemoryRequest` struct. This revealed a ton of weaknesses and flaws in the rest of my memory units and most of them also required a heavy refactor to accommodate the new request structure. I was getting close with the refactor, but during the process I also discovered a number of other issues that would require refactoring major parts of the simulation as well.

I tried my best to tackle everything in time, going so far as to work for over 24 straight right up until the presentation, but I didn't have enough time to fix all of the poor design decisions I had made at the start to accommodate the mesi protocol effectively in the simulation.

## A fake mesi protocol

The current state of the simulator is an almost working pseudo-mesi protocol. The caches snoop on the bus and set their states properly, and invalidate signals have their intended effect of causing a cache to no longer recognize requests to that address as a hit, but they are still write through. The write through also doesn't tap into the event system to manage the time it would take, so all writes to caches take only the normal cache access time to complete. 

Even with this simplification, the cached simulation doesn't seem to produce the right image. It looks close to what it is supposed to be, but it has a regular pattern of dots that seem to have been missed. I think this is a direct result of the coherency protocol not being in place completely. The intermediate image produce by cpu0 matches between both the simulation with no cache and the one with the caches, so I know the caches aren't corrupting data on its way to memory.

## Performance comparison

The default rule for `make` is set up to run both the cacheless and cached simulations. After the cacheless simulation completes it will output a `convolution.txt` and `output.txt` file that represent the data generated from cpu0 and cpu1 respectively. The make rule is configured to also turn those files into `convolution.png` and `output.png` so you can visualize the results. The original data is used to generate `original.png` for comparison.

### Cacheless simulation results

| Cpu  | Clock cycles | Instructions | CPI     |
| ---: | :----------: | :----------: | :------ |
| Cpu0 | 43918378     | 3691516      | 11.8971 |
| Cpu1 | 43918783     | 3791558      | 11.5833 |

After the cacheless simulation completes, the make rule will stop and prompt you to press enter to continue so you can view the results. Then it will run the simulation again, but with both cpus configured with caches. 

The caches for this simulation are configured to use the optimal setup from the last assignment. The cache line size is 128B. Instruction cache is direct mapped with 256B of memory and the data caches are 4 way associative with 512B. The calculated hit time for the instruction cache is 1 sim tick and for the data cache is 8 sim ticks.

### Cached simulation 

| Cpu  | Clock cycles | Instructions | CPI      |
| ---: | :----------: | :----------: | :------- |
| Cpu0 | 9957195      | 3691516      | 2.726941 |
| Cpu1 | 9957275      | 3791558      | 3.731209 |



### Cache statistics summary

| Cache   | Size    | Associativity | Access Time | Hit Rate |
| ------: | :-----: | :-----------: | :---------: | :------- |
| Cpu0 I$ | 256 B   | 1             | 1           | 95.7582% |
| Cpu0 D$ | 512 B   | 4             | 8           | 97.1140% |
| Cpu1 I$ | 256 B   | 1             | 1           | NAN      |
| Cpu1 D$ | 512 B   | 4             | 8           | 89.0241% |

The `nan` result for cpu1's instruction cache is likely due to the problems I was having with the `Fetch` unit. For this simulation it doesn't currently handle canceling the memory request on a pipeline flush very well. I'm not entirely sure what the `nan` result means as I was focused on trying to tackle the memory request problem. The simulation result here is a dirty fix that appears to be breaking the cache, and I suspect only works because all of the branch instructions in the program for cpu1 are followed by a pc+4 jump that is effectively a no-op. Since my "fix" is to drop the instruction in the fetch unit and just increment the program counter, it happens to line up with the behavior of the jump that would have otherwise been executed.

## Final results

Even though the output of the cached version is not quite right, the number of instructions and the time it takes to execute them should be pretty close to the expected values. The write back not taking any time on the caches does make the cpi look better than it really should be though.

Cpu0 went from 11.8971 cpi down to just 2.726941 cpi. Since the output from the convolution operation matches between the two, this number should only be missing the time the caches would have taken for write backs. The addition of the caches to cpu0 reduced the cpi by ~77%;

Cpu1 also had an improvement, but considering that the instruction cache appears to have been completely broken the improvement is probably less reliable. Adding caches to cpu1 reduced the cpi from 11.5833 to 3.731209, about a 67.8% reduction.

The cpu1 cpi is slightly higher than cpu0, even though the cacheless simulation was lower. This is likely due to the hit rate for the data cache being lower. In the cacheless simulation every memory access requires a fully wait, but since cpu0 benefits more from the cache configuration it gets a better cpi reduction.