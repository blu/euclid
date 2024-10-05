An Extremely Naive Prime Factorizer
===================================

I hadn't written an Euclidean factorizer since high school, and my kids are now in primary school doing GCDs and LCMs all the time, so I decided to do a naive prime factorizer *not* using the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes), thus getting a complexity advantage when factoring composites.

Obligatory Random Benchmarking
==============================

Compute the factors to a pre-selected 20-digit number (largest 64bit prime): 18446744073709551557.

Compiler gnu-style CLI:

```bash
$ g++ -std=c++17 -Ofast -fno-rtti -fno-exceptions -fstrict-aliasing -mcpu=<uarch> -mtune=<uarch> main.cpp -o euclid_all64 -DNUM_64BIT=1
$ g++ -std=c++17 -Ofast -fno-rtti -fno-exceptions -fstrict-aliasing -mcpu=<uarch> -mtune=<uarch> main_odd.cpp -o euclid_odd64 -DNUM_64BIT=1
```

Note: at the absence of linux perf, cpu frequency verified via Willy Tarreau's [mhz utility](http://git.1wt.eu/web?p=mhz.git).

| CPU                                             | time, s          | CPU cycles, 1e6  |
| ----------------------------------------------- | ---------------- | ---------------- |
| BCM2712 Cortex-A76 2.4GHz (odd)                 | 12.615           | 30209            |
| T234 Cortex-A78AE 2.2GHz (odd)                  | 13.755           | 30208            |
| Snapdragon X1E-78-100 3.4GHz (odd)              | 1.289            | 4296             |
| Apple M1 P-core 3.2GHz (odd)                    | 1.340            | 4288             |


There is a glaring difference in the results. What's going on here? To fully answer that question we need to start from afar.

Short Background of HW Division
-------------------------------

For the longest of time division was not implemented as an instruction in microprocessors. Eearly microprocessors from the early 1970's did not even feature multiplication, let alone division -- those operations were too taxing in terms of die area and were left to software. Addition, subtraction and bit manipulations comprised all the arithmetics early microprocessors could do. HW integer division was not to be until the arrival of 16-bit microprocessors in the form of Intel 8086 and Motorola 68000 (16/32-bit design) in 1978 and 1979, respectively. In the meantime, 8-bit microprocessors got multiplication in Motorola 6809 from 1978. But that is considerd a tall oder to this day -- 8-bit cpus shy away from multiplication.

So with lithography advancements division eventually found its way into microprocessors, but even today not every 32-bit CPU ISA features division, and even for some of those that do, it is still an optional part of the ISA. Fact of matter is, division is still among the most expensive arithmetics in terms of instruction timing on any given uarch. Integer division is universally characterised by high latency -- result becomes available after many cycles (results can be computed at as low rates as 1 bit per cycle!), and low throughput -- few such ops are executable per unit of time (one DIV unit in the pipeline, the unit iself is not pipelined). Some 32-bit real-time oriented CPUs and MCUs like Arm Ltd's Cortex-R and Cortex-M series feature optimised hw division with relatively low latency, but their throughput is still low. What we observe here in Snapdragon Oryon and Apple M1 results is way, way beyond the capabilities of prior designs, irrespective of ISA.

A Close Look At The Code
------------------------

The compiler universally produces two tight loops from the source factorisation loop:

* One loop seeking the next prime divisor
* Another loop exhausting the newly found prime divisor from the divident

In the context of our benchmark, where we factorize a prime, only the first loop matters.

This is how this loop looks on ARMv8-A64:
```asm
 aa8:   91000a94        add     x20, x20, #0x2
 aac:   9ad40a62        udiv    x2, x19, x20
 ab0:   9b14cc43        msub    x3, x2, x20, x19
 ab4:   eb02029f        cmp     x20, x2
 ab8:   540008a8        b.hi    bcc <main+0x20c>
 abc:   b4000063        cbz     x3, ac8 <main+0x108>
 ac0:   17fffffa        b       aa8 <main+0xe8>
```

* 1st op advances the candidate divisor -- odd candidates only
* 2nd op carries out 64-bit unsigned division
* 3rd op computes the remainder of the division (ARMv8 div does not produce a remainder result)
* 4th op checks if we passed the sqrt(dividend) mark while looking for divisors
* 5th op is the branch out based on the 4th op
* 6th op is a branch to the exhaust loop when a (prime) divisor is found
* 7th op is a branch that unconditionally loops this entire logic again

A fairly tight loop. Tight loops featuring division, especially 64-bit one, are recipes for low op throughput, i.e. low IPC, as witnessed everywhere but in Snapdragon Oryon `perf` results. All other Arm participants we have `perf` results for, even though fairly performant OoO designs, produce an IPC of 0.5 on this test -- one op completed every 2 cycles. Oryon produces an IPC of 3.5 -- 7 times higher! In other words, Oryon completes an entire iteration of this loop every 2 cycles.. What gives?

A Division Unit Like No Other
-----------------------------

Oryon is not just a state-of-the-art IPC OoO design. It also has "special powers" on this test -- an integer DIV unit unlike any other. Well, any other but Apple Silicon. So here we will "quote the brochure" on Apple Silicon P-cores, as of the time of this writing Oryon does not have its instruction timings published yet, and Oryon and AS P-cores results on this test are as close as they come.

```
SDIV, UDIV: 64b latency of 7-9 cycles. A new divide can be issued every other cycle.
```

Not only is the latency unprecedentedly low, but the throughput is also high as the unit is fully pipelined -- one division issued every 2 cycles! Unreal.

The throughput is of particular importace to our test. Remember Oryon achieves one iteration every 2 cycles? Well, this is a major why.

Rant on The Importance of Following Standards
---------------------------------------------

Profiling on AS is not trivial. While Linux `perf` is the state-of-the-art in PMU-based profiles across all architectures, and AS has a viable PMU, there's no `perf` equivalent on macOS. There is `Instruments` there but it is far from `perf` feature-wise. Particularly `perf stat`-wise. So just run a guest Linux, no? Nope. Apple's Hypervisor Framework does not seem to expose any of the PMU machinery to guests. The only viable way to use `perf` on AS is in Asahi Linux, thanks to the enormous effort the Asahi team has invested in reverse engineering the plethora of undocumented bits and pieces of AS. Kudos to them. But that shouldn't have been needed PMU-wise, had Apple implemented one of the ARMv8 ISA standard PMU extensions, like e.g. PMUv3 which has been the baseline for more than a decade now. Instead Apple decided to stick to their in-house PMU and not share it with guests. Thanks Zeus Oryon was designed as a proper OEM, Linux-capable part, so even WSL2 guests get standard ARMv8 PMU features. In the meantime Apple can continue playing the Knights who say NIH. /rant over

Anyhow, we can use the readings from `perf`-equipped platforms to draw some conclusions about M1 P-core IPC. As we use g++ to compile for M1, the codegen yields the same 7-op loop, save for trivialities like `cmp A, B; b.hi` <-> `cmp B, A; b.lo`.

What we know is that the 7-op, strictly-compute loop universally takes 15,033 Mops to conclude, given this argument. We also know (indirectly, time * clock) that the duration of the loop is 4288 Mcycles on M1 P-core. That translates to an IPC of 3.5058 -- virtually the same as Oryon.

Final Words
-----------

How uarch architects spend their transistor budgets is perhaps the raison d'etre of that profession. But we can all agree that we have a solid proof that state-of-the-art general-purpose processors can afford *not* to botch their integer-division implementions, and give that op a first-class citizenship instead. As somebody who remembers the times when division and multiplication were entirely sw devs' problems, I am happy that somebody has finally taken the initiative to solve that basic problem at a much more fundamental level. Thank you, Oryon and AS P-core architects!

Benchmark Logs (best results from a possible multi-run)
=======================================================

BCM2712 Cortex-A76 @ 2.4GHz, armv8-a64, g++-12.2.0
-----------------------------------------------------------------------------------

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,branches:u,branch-misses:u ./euclid_all64 12345678901234567
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
$ perf stat -e task-clock:u,cycles:u,instructions:u,branches:u,branch-misses:u ./euclid_odd64 12345678901234567
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

```
$ perf stat -e task-clock:u,cycles:u,instructions:u,branches:u,branch-misses:u ./euclid_odd64 18446744073709551557
prime: 18446744073709551557, power: 1 (18446744073709551557)

 Performance counter stats for './euclid_odd64 18446744073709551557':

         12,613.18 msec task-clock:u                     #    1.000 CPUs utilized             
    30,208,768,999      cycles:u                         #    2.395 GHz                       
    15,032,488,410      instructions:u                   #    0.50  insn per cycle            
     6,442,473,464      branches:u                       #  510.773 M/sec                     
             1,984      branch-misses:u                  #    0.00% of all branches           

      12.614879760 seconds time elapsed

      12.614909000 seconds user
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

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,br_retired:u,br_mis_pred_retired:u ./euclid_odd64 18446744073709551557
prime: 18446744073709551557, power: 1 (18446744073709551557)

 Performance counter stats for './euclid_odd64 18446744073709551557':

         13,754.55 msec task-clock:u              #    1.000 CPUs utilized          
    30,208,253,262      cycles:u                  #    2.196 GHz                    
    15,032,539,511      instructions:u            #    0.50  insn per cycle         
     6,442,489,666      br_retired:u              #  468.390 M/sec                  
             4,069      br_mis_pred_retired:u     #    0.296 K/sec                  

      13.755350986 seconds time elapsed

      13.749115000 seconds user
       0.000000000 seconds sys
```

Snapdragon X1E-78-100 @ 3.4GHz, armv8-a64, g++-11.4.0
-----------------------------------------------------------------------------------

```bash
$ perf stat -e task-clock:u,cycles:u,instructions:u,br_retired:u,br_mis_pred_retired:u ./euclid_odd64 18446744073709551557
prime: 18446744073709551557, power: 1 (18446744073709551557)

 Performance counter stats for './euclid_odd64 18446744073709551557':

           1288.56 msec task-clock:u              #    1.000 CPUs utilized
        4296479355      cycles:u                  #    3.334 GHz
       15033998641      instructions:u            #    3.50  insn per cycle
        6442774480      br_retired:u              #    5.000 G/sec
              9863      br_mis_pred_retired:u     #    7.654 K/sec

       1.289171165 seconds time elapsed

       1.283266000 seconds user
       0.003991000 seconds sys
```

Apple M1 @ 3.2GHz, armv8-a64, g++-13.0.0
-----------------------------------------------------------------------------------

```bash
% for i in {0..15} ; do /usr/bin/time -p ./euclid_odd64 18446744073709551557 ; done
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.40
user         1.37
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
prime: 18446744073709551557, power: 1 (18446744073709551557)
real         1.34
user         1.34
sys          0.00
% ./mhz 16
count=1576275 us50=24615 us250=123206 diff=98591 cpu_MHz=3197.604
count=1576275 us50=24936 us250=123316 diff=98380 cpu_MHz=3204.462
count=1576275 us50=24750 us250=123482 diff=98732 cpu_MHz=3193.038
count=1576275 us50=24817 us250=123232 diff=98415 cpu_MHz=3203.323
count=1576275 us50=24759 us250=123326 diff=98567 cpu_MHz=3198.383
count=1576275 us50=24752 us250=123426 diff=98674 cpu_MHz=3194.915
count=1576275 us50=24796 us250=123273 diff=98477 cpu_MHz=3201.306
count=1576275 us50=24794 us250=123593 diff=98799 cpu_MHz=3190.872
count=1576275 us50=24830 us250=123236 diff=98406 cpu_MHz=3203.616
count=1576275 us50=24775 us250=123402 diff=98627 cpu_MHz=3196.437
count=1576275 us50=24719 us250=123238 diff=98519 cpu_MHz=3199.941
count=1576275 us50=24757 us250=123256 diff=98499 cpu_MHz=3200.591
count=1576275 us50=24734 us250=123458 diff=98724 cpu_MHz=3193.296
count=1576275 us50=24793 us250=123322 diff=98529 cpu_MHz=3199.616
count=1576275 us50=24755 us250=123651 diff=98896 cpu_MHz=3187.743
count=1576275 us50=24802 us250=123244 diff=98442 cpu_MHz=3202.444
```
