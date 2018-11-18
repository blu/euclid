An Extremely Naive Prime Factorizer
===================================

I haven't written an Euclidean factorizer since high school, and my kids are now in primary school doing GCDs and LCMs all the time, so I decided to do a naive prime factorizer *not* using the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes), thus getting a complexity advantage when factoring composites. Thanks to its extreme simplicity code could also be used as a teaching example to young students.

Obligatory Random Benchmarking Results
======================================

Compute the factors to the 1,000,000th prime, effectively finding all primes up to the 1,000,000th one. Naturally, ISAs featuring integer division perform *much* better than those without.

Note: at the absence of linux perf, cpu frequency verified via Willy Tarreau's [mhz utility](http://git.1wt.eu/web?p=mhz.git).


Cortex-A53 @ 1.7GHz -- aarch64-a32 (integer division; core part of bigLITTLE)
-----------------------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mcpu=cortex-a53 -mtune=cortex-a53 -DBENCHMARK main.cpp
$ time taskset 0x3 ./a.out 15485863
prime: 15485863, power: 1

real    0m4.189s
user    0m4.172s
sys     0m0.012s
$ echo "scale=4; 4.189 * 1.7" | bc
7.1213
$ taskset 0x3 ./mhz
count=645643 us50=19433 us250=94987 diff=75554 cpu_MHz=1709.090
```

Cortex-A72 @ 2.1GHz -- aarch64-a32 (integer division; core part of bigLITTLE)
-----------------------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mcpu=cortex-a57 -mtune=cortex-a57 -DBENCHMARK main.cpp
$ time taskset 0xc ./a.out 15485863
prime: 15485863, power: 1

real    0m2.300s
user    0m2.284s
sys     0m0.012s
$ echo "scale=4; 2.300 * 2.1" | bc
4.8300
$ taskset 0xc ./mhz
count=1008816 us50=23979 us250=119882 diff=95903 cpu_MHz=2103.826
```

Cortex-A72 @ 1.3GHz -- aarch64-a64 (integer divison)
----------------------------------------------------

```
$ clang++-4.0 -Ofast -fno-rtti -fno-exceptions -mtune=cortex-a57 -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       3697.448000      task-clock (msec)         #    1.000 CPUs utilized
        4791578640      cycles:u                  #    1.296 GHz
        4623077694      instructions:u            #    0.96  insns per cycle

       3.698267891 seconds time elapsed
```

Xeon E3-1270v2 IVB @ 1.6GHz / 3.9GHz -- x86-64 (integer division)
-----------------------------------------------------------------

```
$ g++-6 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       4055.741930 task-clock (msec)         #    0.999 CPUs utilized
     6,469,206,597 cycles:u                  #    1.595 GHz
     5,694,763,911 instructions:u            #    0.88  insns per cycle

       4.059659905 seconds time elapsed

$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       1664.131312 task-clock (msec)         #    0.999 CPUs utilized
     6,469,103,707 cycles:u                  #    3.887 GHz
     5,694,762,569 instructions:u            #    0.88  insns per cycle

       1.665749981 seconds time elapsed
```

Baikal-T1 p5600 @ 1.2GHz -- mips32r5 (integer division)
-------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=p5600 -mtune=p5600 -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       6766.543120      task-clock (msec)         #    0.999 CPUs utilized
        8099572959      cycles:u                  #    1.197 GHz
        5690154495      instructions:u            #    0.70  insns per cycle

       6.775047190 seconds time elapsed
```

Xeon E5-2687W SNB @ 3.1GHz -- x86-64 (integer division)
-------------------------------------------------------

```
$ g++-4.8 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       2407.800658      task-clock (msec)         #    0.999 CPUs utilized
     7,448,537,171      cycles:u                  #    3.094 GHz
     5,657,934,987      instructions:u            #    0.76  insns per cycle

       2.410431996 seconds time elapsed
```

Core i7-4770 HSW @ 3.9GHz -- x86-64 (integer division)
------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=haswell -mtune=haswell -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       1668.399983      task-clock (msec)         #    1.000 CPUs utilized
        6455080866      cycles:u                  #    3.869 GHz
        5690572016      instructions:u            #    0.88  insn per cycle

       1.668583606 seconds time elapsed
```
