# Assignment 4

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

The best performance with respect to total time taken was `Cache 2`. It has a slightly longer access time than `Cache 1` but makes up for it by increasing the hit rate significantly. With the least amount of time spent accessing memory, `Cache 2` comes out on top.

The next best performing cache with respect to total time is `Cache 3`. This case is interesting because it actually has the worse hit rate. Out of all the caches it spends the most time fetching data from memory. However since it has the highest block size, and therefore the lowest number of blocks to check, it has the lowest `access time`. Hits inside this cache are significantly faster than any of the other caches. This boost in access time on a hit ends up outweighing a lot of the misses it incurs from replacing blocks that will be accessed again. These subsequent accesses form a significant portion of the misses for `Cache 3` since it has the highest `miss rate`, but the lowest `compulsory miss rate`.

The baseline `Cache 1` is third ranked in this list. It is slower than `Cache 2` because it is smaller and therefore has to replace blocks more often. It's access time is faster than `Cache 2` but that gets outweighed by the extra time spent on pulling blocks from memory. `Cache 3` is faster than `Cache 1` for almost exactly the opposite reason. They both are the same size, but `Cache 3` has a faster access time due to having larger blocks. The `hit rate` between `Cache 1` and `Cache 3` remains almost identical, but the longer access time in `Cache 1` makes it significantly slower than `Cache 3`.

The slowest cache overall was `Cache 4` which was identical to `Cache 1` in every respect except the `associativity`. The added associativity increased the access time of the cache dramatically. Even though the `hit rate` was improved by 2% it wasn't enough to merit a 400% increase in access time. Where the other caches are 25-50X faster than the memory in this simulation, `Cache 4` is actually less than 10X faster. This means that even cache hits take a relatively long amount of time compared to the other caches. With a very limited amount of space to work with, the added associativity isn't able to increase the hit rate significantly enough to make the added access time worth it.

Overall it seems that finding a balance between a nice hit rate while keeping a minimal access time works best for the caches in this simulation. 