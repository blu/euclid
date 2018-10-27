An Extremely Naive Prime Factorizer
===================================

I haven't written an Euclidean factorizer since high school, and my kids are now in primary school doing GCDs and LCMs all the time, so I decided to do a naive prime factorizer *not* using the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes), thus getting a complexity advantage when factoring composites. Thanks to its extreme simplicity code could also be used as a teaching example to young students.

Obligatory Random Benchmarking Results
======================================

Compute the factors to the 1,000,000th prime, effectively finding all primes up to the 1,000,000th one. Naturally, ISAs featuring integer division perform *much* better than those without.


Cortex-A72 @ 1.3GHz -- aarch64 (integer divison)
------------------------------------------------

```
$ clang++-4.0 -Ofast -fno-rtti -fno-exceptions -mtune=cortex-a57 -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       4353.220880      task-clock (msec)         #    1.000 CPUs utilized
        5642946243      cycles:u                  #    1.296 GHz
        5663352558      instructions:u            #    1.00  insns per cycle

       4.354036748 seconds time elapsed
```

Xeon E3-1270v2 IVB @ 1.6GHz / 3.9GHz -- x86-64 (integer division)
-----------------------------------------------------------------

```
$ clang++-3.7 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       5226.616569 task-clock (msec)         #    0.999 CPUs utilized
     8,338,778,205 cycles:u                  #    1.595 GHz
     6,978,311,461 instructions:u            #    0.84  insns per cycle

       5.231523741 seconds time elapsed

$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       2146.105392 task-clock (msec)         #    0.999 CPUs utilized
     8,338,813,115 cycles:u                  #    3.886 GHz
     6,978,310,683 instructions:u            #    0.84  insns per cycle

       2.148160824 seconds time elapsed
```

Baikal-T1 p5600 @ 1.2GHz -- mips32r5 (integer division)
-------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=p5600 -mtune=p5600 -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       7552.067520      task-clock (msec)         #    1.000 CPUs utilized
        9041600014      cycles:u                  #    1.197 GHz
        6295603111      instructions:u            #    0.70  insns per cycle

       7.554098610 seconds time elapsed

$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=p5600 -mtune=p5600 -DBENCHMARK main_alt_alt.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       7496.085560      task-clock (msec)         #    1.000 CPUs utilized
        8978995370      cycles:u                  #    1.198 GHz
        5710222094      instructions:u            #    0.64  insns per cycle

       7.497204873 seconds time elapsed

```

Xeon E5-2687W SNB @ 3.1GHz -- x86-64 (integer division)
-------------------------------------------------------

```
$ clang++-3.9 -Ofast -fno-rtti -fno-exceptions -march=native -mtune=native -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       2875.980559      task-clock (msec)         #    0.999 CPUs utilized
     8,900,021,986      cycles:u                  #    3.095 GHz
     6,913,241,124      instructions:u            #    0.78  insns per cycle

       2.879061843 seconds time elapsed
```

Core i7-4770 HSW @ 3.9GHz -- x86-64 (integer division)
------------------------------------------------------

```
$ g++-7.3 -Ofast -fno-rtti -fno-exceptions -march=haswell -mtune=haswell -DBENCHMARK main.cpp
$ perf stat -e task-clock,cycles:u,instructions:u -- ./a.out 15485863
prime: 15485863, power: 1

 Performance counter stats for './a.out 15485863':

       2129.954573      task-clock (msec)         #    1.000 CPUs utilized
     8,246,655,732      cycles:u                  #    3.872 GHz
     6,948,639,247      instructions:u            #    0.84  insn per cycle

       2.130177734 seconds time elapsed
```
