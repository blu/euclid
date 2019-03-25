An Extremely Naive Prime Factorizer
===================================

I haven't written an Euclidean factorizer since high school, and my kids are now in primary school doing GCDs and LCMs all the time, so I decided to do a naive prime factorizer *not* using the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes), thus getting a complexity advantage when factoring composites. Thanks to its extreme simplicity code could also be used as a teaching example to young students.

Obligatory Random Benchmarking Results
======================================

Compute the factors to the 1,000,000th prime, effectively finding all primes up to the 1,000,000th one. Naturally, ISAs featuring integer division perform *much* better than those without.

Note: at the absence of linux perf, cpu frequency verified via Willy Tarreau's [mhz utility](http://git.1wt.eu/web?p=mhz.git).

| CPU                                             | time, s          | CPU cycles, 10e9 |
| ----------------------------------------------- | ---------------- | ---------------- |
| MT8173C Cortex-A53 @1.7GHz, armv8-a32           | 4.181            | 7.108            |
| MT8173C Cortex-A72 @2.1GHz, armv8-a32           | 2.299            | 4.828            |
| RK3399 Cortex-A72 @2.0GHz, armv8-a32            | 2.428            | 4.856            |
| AWS Graviton Cortex-A72 @2.29GHz, armv8-a64     | 2.102            | 4.816            |
| ARMADA 8040 Cortex-A72 @1.3GHz, armv8-a64       | 3.694            | 4.787            |
| Intel Xeon E5-2687W @3.1GHz, x86-64             | 2.409            | 7.445            |
| Intel Xeon E3-1270v2 @1.6GHz, x86-64            | 4.059            | 6.468            |
| Intel Xeon E3-1270v2 @3.9GHz, x86-64            | 1.665            | 6.467            |
| Intel Core i7-4770 @3.9GHz, x86-64              | 1.665            | 6.453            |
| Intel Xeon Platinum 8175M @2.5GHz, x86-64       | 1.451            | 3.628            |
| Baikal-T1 p5600 @1.2GHz, mips32r5               | 6.767            | 8.099            |

Benchmark Logs
==============

MT8173C Cortex-A53 @ 1.7GHz -- armv8-a32 (integer division; core part of bigLITTLE)
-----------------------------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mcpu=cortex-a53 -mtune=cortex-a53 main_bench.cpp
$ time taskset 0x3 ./a.out 15485863
prime: 15485863, power: 1

real    0m4.181s
user    0m4.156s
sys     0m0.024s
$ echo "scale=4; 4.181 * 1.7" | bc
7.1077
$ taskset 0x3 ./mhz
count=645643 us50=19433 us250=94987 diff=75554 cpu_MHz=1709.090
```

MT8173C Cortex-A72 @ 2.1GHz -- armv8-a32 (integer division; core part of bigLITTLE)
-----------------------------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mcpu=cortex-a57 -mtune=cortex-a57 main_bench.cpp
$ time taskset 0xc ./a.out 15485863
prime: 15485863, power: 1

real    0m2.299s
user    0m2.280s
sys     0m0.016s
$ echo "scale=4; 2.299 * 2.1" | bc
4.8279
$ taskset 0xc ./mhz
count=1008816 us50=23979 us250=119882 diff=95903 cpu_MHz=2103.826
```

RK3399 Cortex-A72 @ 2.0GHz -- armv8-a32 (integer division; core part of bigLITTLE)
----------------------------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mcpu=cortex-a57 -mtune=cortex-a57 main_bench.cpp
$ time taskset 0x30 ./a.out 15485863
prime: 15485863, power: 1

real    0m2.428s
user    0m2.399s
sys     0m0.028s
$ echo "scale=4; 2.428 * 2.0" | bc
4.8560
$ taskset 0x30 ./mhz
count=807053 us50=20146 us250=100541 diff=80395 cpu_MHz=2007.719
```

AWS Graviton Cortex-A72 @ 2.29GHz -- armv8-a64 (integer division)
-----------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mtune=cortex-a57 main_bench.cpp
$ time ./a.out 15485863
prime: 15485863, power: 1

real    0m2.102s
user    0m2.086s
sys     0m0.016s
$ echo "scale=4; 2.102 * 2.29" | bc
4.8135
$ ./mhz
count=1008816 us50=22005 us250=110186 diff=88181 cpu_MHz=2288.058
```

Marvell ARMADA 8040 Cortex-A72 @ 1.3GHz -- armv8-a64 (integer divison)
----------------------------------------------------------------------

```
$ g++-5 -Ofast -fno-rtti -fno-exceptions -mtune=cortex-a57 main_bench.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       3693.712520      task-clock (msec)         #    1.000 CPUs utilized
        4786538472      cycles:u                  #    1.296 GHz
        4658586145      instructions:u            #    0.97  insns per cycle

       3.694419028 seconds time elapsed
```

Xeon Platinum 8175M SKL @ 2.5GHz -- x86-64 (integer division)
-------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=skylake -mtune=skylake main_bench.cpp
$ time ./a.out 15485863
prime: 15485863, power: 1

real    0m1.451s
user    0m1.447s
sys     0m0.004s
$ echo "scale=4; 1.451 * 2.5" | bc
3.6275
$ ./mhz
count=1261020 us50=20373 us250=101857 diff=81484 cpu_MHz=3095.135 tsc50=50930432 tsc250=254640864 diff=161 rdtsc_MHz=2500.005
```

Xeon E3-1270v2 IVB @ 1.6GHz / 3.9GHz -- x86-64 (integer division)
-----------------------------------------------------------------

```
$ g++-6 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native main_bench.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       4055.106984 task-clock (msec)         #    0.999 CPUs utilized
     6,468,053,560 cycles:u                  #    1.595 GHz
     5,695,763,912 instructions:u            #    0.88  insns per cycle

       4.059041995 seconds time elapsed

$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       1663.571983 task-clock (msec)         #    0.999 CPUs utilized
     6,466,742,401 cycles:u                  #    3.887 GHz
     5,695,956,801 instructions:u            #    0.88  insns per cycle

       1.665173340 seconds time elapsed
```

Baikal-T1 p5600 @ 1.2GHz -- mips32r5 (integer division)
-------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=p5600 -mtune=p5600 main_bench.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       6765.938080      task-clock (msec)         #    1.000 CPUs utilized
        8099441679      cycles:u                  #    1.197 GHz
        5693154905      instructions:u            #    0.70  insns per cycle

       6.766798895 seconds time elapsed
```

Xeon E5-2687W SNB @ 3.1GHz -- x86-64 (integer division)
-------------------------------------------------------

```
$ g++-4.8 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native main_bench.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       2406.310097      task-clock (msec)         #    0.999 CPUs utilized
     7,445,442,582      cycles:u                  #    3.094 GHz
     5,659,934,967      instructions:u            #    0.76  insns per cycle

       2.408957627 seconds time elapsed
```

Core i7-4770 HSW @ 3.9GHz -- x86-64 (integer division)
------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=haswell -mtune=haswell main_bench.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       1665.148192      task-clock (msec)         #    1.000 CPUs utilized
        6453417692      cycles:u                  #    3.876 GHz
        5690572110      instructions:u            #    0.88  insn per cycle

       1.665336581 seconds time elapsed
```
