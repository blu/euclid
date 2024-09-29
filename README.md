An Extremely Naive Prime Factorizer
===================================

I hadn't written an Euclidean factorizer since high school, and my kids are now in primary school doing GCDs and LCMs all the time, so I decided to do a naive prime factorizer *not* using the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes), thus getting a complexity advantage when factoring composites.

Obligatory Random Benchmarking Results
======================================

Compute the factors to a pre-selected 17-digit number (64bit): 12345678901234567.

Compiler gnu-style CLI:

```bash
$ g++ -Ofast -fno-rtti -fno-exceptions -mcpu=<isa> -mtune=<isa> main.cpp -o euclid_all64 -DNUM_64BIT=1
$ g++ -Ofast -fno-rtti -fno-exceptions -mcpu=<isa> -mtune=<isa> main_odd.cpp -o euclid_odd64 -DNUM_64BIT=1
```

Note: at the absence of linux perf, cpu frequency verified via Willy Tarreau's [mhz utility](http://git.1wt.eu/web?p=mhz.git).

| CPU                                             | time, ms         | CPU cycles, 1e6  |
| ----------------------------------------------- | ---------------- | ---------------- |
| BCM2712 Cortex-A76 2.4GHz (all)                 | 215              | 514              |
| BCM2712 Cortex-A76 2.4GHz (odd)                 | 108              | 258              |
| T234 Cortex-A78AE 2.2GHz (all)                  | 236              | 514              |
| T234 Cortex-A78AE 2.2GHz (odd)                  | 120              | 258              |

Benchmark Logs (best results from a possible multi-run)
=======================================================

BCM2712 Cortex-A76 @ 2.4GHz, armv8-a64, g++-12.2.0
-----------------------------------------------------------------------------------

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,branches:u,branch-misses:u ./euclid_all64_gcc 12345678901234567
prime: 7, power: 1 (7)
prime: 1763668414462081, power: 1 (1763668414462081)

 Performance counter stats for './euclid_all64_gcc 12345678901234567':

            214.93 msec task-clock:u                     #    0.998 CPUs utilized             
       514,345,644      cycles:u                         #    2.393 GHz                       
       295,317,919      instructions:u                   #    0.57  insn per cycle            
       126,272,544      branches:u                       #  587.493 M/sec                     
            10,143      branch-misses:u                  #    0.01% of all branches           

       0.215285783 seconds time elapsed

       0.215341000 seconds user
       0.000000000 seconds sys
```

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,branches:u,branch-misses:u ./euclid_odd64_gcc 12345678901234567
prime: 7, power: 1 (7)
prime: 1763668414462081, power: 1 (1763668414462081)

 Performance counter stats for './euclid_odd64_gcc 12345678901234567':

            107.96 msec task-clock:u                     #    0.998 CPUs utilized             
       257,855,069      cycles:u                         #    2.388 GHz                       
       148,331,724      instructions:u                   #    0.58  insn per cycle            
        63,278,464      branches:u                       #  586.111 M/sec                     
             9,561      branch-misses:u                  #    0.02% of all branches           

       0.108223428 seconds time elapsed

       0.108320000 seconds user
       0.000000000 seconds sys
```

Tegra T234 Cortex-A78 @ 2.2GHz, armv8-a64, g++-10.5.0
-----------------------------------------------------------------------------------

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,br_retired:u,br_mis_pred_retired:u ./euclid_all64 12345678901234567
prime: 7, power: 1 (7)
prime: 1763668414462081, power: 1 (1763668414462081)

 Performance counter stats for './euclid_all64 12345678901234567':

            235.24 msec task-clock:u              #    0.996 CPUs utilized          
       514,341,917      cycles:u                  #    2.186 GHz                    
       295,564,406      instructions:u            #    0.57  insn per cycle         
       126,320,176      br_retired:u              #  536.981 M/sec                  
            11,679      br_mis_pred_retired:u     #    0.050 M/sec                  

       0.236098786 seconds time elapsed

       0.235930000 seconds user
       0.000000000 seconds sys
```

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,br_retired:u,br_mis_pred_retired:u ./euclid_odd64 12345678901234567
prime: 7, power: 1 (7)
prime: 1763668414462081, power: 1 (1763668414462081)

 Performance counter stats for './euclid_odd64 12345678901234567':

            118.99 msec task-clock:u              #    0.994 CPUs utilized          
       258,165,755      cycles:u                  #    2.170 GHz                    
       148,578,230      instructions:u            #    0.58  insn per cycle         
        63,326,100      br_retired:u              #  532.206 M/sec                  
            11,599      br_mis_pred_retired:u     #    0.097 M/sec                  

       0.119667683 seconds time elapsed

       0.119447000 seconds user
       0.000000000 seconds sys
```
