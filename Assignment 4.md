# Assignment 4 Part 1

## Cache comparison

For comparing the cache performance, there were 4 set cache configurations tested.

| Simulation | Cache size | Cache line size | Associativity | Cache hit time |
| :--------- | :--------: | :-------------: | :-----------: | :------------: |
| Cache 1   | 256B        | 32B             | Direct Mapped | 3              |
| Cache 2   | 512B        | 32B             | Direct Mapped | 4              |
| Cache 3   | 256B        | 64B             | Direct Mapped | 2              |
| Cache 4   | 256B        | 32B             | 4-way         | 12             |


### Simulation results
The four simulations can be executed with the trace file by executing `cache_performance_test.cpp`. The project is set up so that `make cache_performance_test` will compile an execute the test.

The results of the simulations ordered by ticks elapsed is as follows:

| Simulation | Cache access time | Hit rate | Compulsory miss rate | Total ticks elapsed |
| :--------- | :---------------: | :------: | :------------------: | :------------------ |
| Cache 2    | 4                 | 74.0%    | 9.3%                 | 30000               |
| Cache 3    | 2                 | 68.6%    | 5.2%                 | 33400               |
| Cache 1    | 3                 | 68.8%    | 9.3%                 | 34200               |
| Cache 4    | 12                | 70.5%    | 9.3%                 | 41500               |

For the comparisons I'll be assuming `Cache 1` is the baseline to compare against since each of the other simulation caches had only one variation from the cache in simulation 1.

### Cache 2
The best performance with respect to total time taken was `Cache 2`. It has a slightly longer access time than `Cache 1` but makes up for it by increasing the hit rate significantly. With the least amount of time spent accessing memory, `Cache 2` comes out on top.

### Cache 3
The next best performing cache with respect to total time is `Cache 3`. This case is interesting because it actually has the worse hit rate. Out of all the caches it spends the most time fetching data from memory. However since it has the highest block size, and therefore the lowest number of blocks to check, it has the lowest `access time`. Hits inside this cache are significantly faster than any of the other caches. This boost in access time on a hit ends up outweighing a lot of the misses it incurs from replacing blocks that will be accessed again. These subsequent accesses form a significant portion of the misses for `Cache 3` since it has the highest `miss rate`, but the lowest `compulsory miss rate`.

### Cache 1
The baseline `Cache 1` is third ranked in this list. It is slower than `Cache 2` because it is smaller and therefore has to replace blocks more often. It's access time is faster than `Cache 2` but that gets outweighed by the extra time spent on pulling blocks from memory. `Cache 3` is faster than `Cache 1` for almost exactly the opposite reason. They both are the same size, but `Cache 3` has a faster access time due to having larger blocks. The `hit rate` between `Cache 1` and `Cache 3` remains almost identical, but the longer access time in `Cache 1` makes it significantly slower than `Cache 3`.

### Cache 4
The slowest cache overall was `Cache 4` which was identical to `Cache 1` in every respect except the `associativity`. The added associativity increased the access time of the cache dramatically. Even though the `hit rate` was improved by 2% it wasn't enough to merit a 400% increase in access time. Where the other caches are 25-50X faster than the memory in this simulation, `Cache 4` is actually less than 10X faster. This means that even cache hits take a relatively long amount of time compared to the other caches. With a very limited amount of space to work with, the added associativity isn't able to increase the hit rate significantly enough to make the added access time worth it.

### Summary
Overall it seems that finding a balance between a nice hit rate while keeping a minimal access time works best for the caches in this simulation.

## Improved Cache

The final section of part 1 is to design a better 256B cache. To do this I wrote a quick program to run through all the possible cache line size and associativity combinations.

From the results of the 4 tests above I expected the best cache to have a minimal access time with a high hit rate. Increasing the cache line size seems to have had the greatest impact on the performance of the cache without increasing its size, so making the line size bigger would make sense. The question is where that would stop being effective, and if at the larger sizes any associativity would be useful. The next size up is 126B and that's half the cache. Without associativity there number of conflicts could be pretty high with only two available slots.

I won't paste the complete results here, but you can answer `y` to the prompt at the end of a `make cache_performance_test` run to run your own analysis. I'll discuss the 256B cache configurations below, but the analyzer is capable of accepting the desired cache size as an input. 

### Top configurations

| Line Size  | Associativity | Cache access time | Hit rate | Compulsory miss rate | Total ticks elapsed |
| :--------- | :-----------: | :---------------: | :------: | :------------------: | :------------------ |
| 128        | 2             | 2                 | 71.8%    | 2.7%                 | 30200               |
| 64         | 2             | 4                 | 72.2%    | 5.2%                 | 31800               |
| 64         | 1             | 2                 | 68.6%    | 5.2%                 | 33400               |
| 32         | 1             | 3                 | 68.8%    | 9.3%                 | 34200               |
| 128        | 1             | 1                 | 65.3%    | 2.7%                 | 35700               |
| 16         | 1             | 4                 | 67.1%    | 14.2%                | 36900               |
| 32         | 2             | 6                 | 69.1%    | 9.3%                 | 36900               |
| 64         | 4             | 8                 | 68.6%    | 5.2%                 | 39400               |

The final determination was that a 2-way associative cache with line size 128B was the most effective. This did not have the highest hit rate, but it was very close to the highest hit rate. It did have close the the lowest access time (only beaten out by the 128B direct mapped version).

This result makes sense because the 128B block size provided the lowest compulsory hit rate aside from the 256B line size (a one block cache) that had an abysmal hit rate. Without associativity the 128B block size hit rate suffers more than the 64B or 32B sizes. But with an associativity of just 2 the 128B block size becomes a fully associative cache since there are only two blocks in the cache. Combined with the pseudo-LRU policy to improve the chances of unneeded blocks being replaced, the hit rate for the 128B block size recovers enough to compete with the 64B associative version. Adding the associativity also only increases the access time by a single cycle, so the benefit from doing so massively outweighs the cost.

An interesting note is that the fully associative 258B cache with 128B line size performs almost identically to the direct mapped 512B cache with line size 32B from the fixed simulations in terms of execution time even though the access times, hit rates, and compulsory miss rates are very different.

# Assignment 4 Part 2

For part 2 of the assignment the goal is to run the dual core simulation from the last assignment with an increased memory access day and various cache configurations. The first run will contain no cache mechanism and act like a baseline. The second run will contain a specified cache configuration. The third run will be an attempt to create an optimal cache configuration for the program being executed.

## Baseline test

With the memory latency updated to be 100 simulation ticks, the CPI of both cpus in the simulation takes a huge hit.

The following data can be obtained by executing `make duo-core` to run the simulation with no caches configured.

| Cpu  | Clock cycles | Instructions | CPI     |
| ---: | :----------: | :----------: | :------ |
| Cpu1 | 71142        | 6160         | 11.549  |
| Cpu2 | 71132        | 6160         | 11.5474 |
