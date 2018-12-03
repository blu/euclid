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

real    0m4.181s
user    0m4.156s
sys     0m0.024s
$ echo "scale=4; 4.181 * 1.7" | bc
7.1077
$ taskset 0x3 ./mhz
count=645643 us50=19433 us250=94987 diff=75554 cpu_MHz=1709.090
```

Cortex-A72 @ 2.1GHz -- aarch64-a32 (integer division; core part of bigLITTLE)
-----------------------------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -mcpu=cortex-a57 -mtune=cortex-a57 -DBENCHMARK main.cpp
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

Cortex-A72 @ 1.3GHz -- aarch64-a64 (integer divison)
----------------------------------------------------

```
$ g++-5 -Ofast -fno-rtti -fno-exceptions -mtune=cortex-a57 -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       3693.712520      task-clock (msec)         #    1.000 CPUs utilized
        4786538472      cycles:u                  #    1.296 GHz
        4658586145      instructions:u            #    0.97  insns per cycle

       3.694419028 seconds time elapsed
```

Xeon E3-1270v2 IVB @ 1.6GHz / 3.9GHz -- x86-64 (integer division)
-----------------------------------------------------------------

```
$ g++-6 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native -DBENCHMARK main.cpp
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
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=p5600 -mtune=p5600 -DBENCHMARK main.cpp
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
$ g++-4.8 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native -DBENCHMARK main.cpp
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
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=haswell -mtune=haswell -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       1665.148192      task-clock (msec)         #    1.000 CPUs utilized
        6453417692      cycles:u                  #    3.876 GHz
        5690572110      instructions:u            #    0.88  insn per cycle

       1.665336581 seconds time elapsed
```
