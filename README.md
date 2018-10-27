An Extremely Naive Prime Factorizer
===================================

I haven't written an Euclidean factorizer since high school, and my kids are now in primary school doing GCDs and LCMs all the time, so I decided to do a naive prime factorizer using the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes). Thanks to its extreme simplicity code could also be used as a teaching example to young students.

Obligatory Random Benchmarking Results
======================================

Compute the factors to the 10,000th prime, effectively finding all primes up to the 10,000th one. Naturally, ISAs featuring integer division perform *much* better than those without.


Cortex-A72 @ 1.3GHz -- aarch64 (integer divison)
------------------------------------------------

```
$ g++-5 main.cpp -Ofast -mtune=cortex-a57 -DBENCHMARK
$ time ./a.out 104729 
prime: 104729, power: 1

real    0m0.546s
user    0m0.544s
sys     0m0.000s
$ echo "scale=4; 0.546 * 1.3" | bc
.7098
```

Cortex-A72 @ 2.1GHz -- armv7 (no integer division)
--------------------------------------------------

```
$ g++-7.3 main.cpp -marm -Ofast -DBENCHMARK
$ time taskset 0xc ./a.out 104729
prime: 104729, power: 1

real    0m1.295s
user    0m1.284s
sys     0m0.000s
$ echo "scale=4; 1.295 * 2.1" | bc
2.7195
```

Cortex-A53 @ 1.7GHz -- armv7 (no integer division)
--------------------------------------------------

```
$ g++-7.3 main.cpp -marm -Ofast -DBENCHMARK
$ time taskset 0x3 ./a.out 104729
prime: 104729, power: 1

real    0m3.117s
user    0m3.112s
sys     0m0.004s
$ echo "scale=4; 3.117 * 1.7" | bc
5.2989
```

Xeon E3-1270v2 IVB @ 1.6GHz -- x86-64 (integer division)
--------------------------------------------------------

```
$ g++-5 main.cpp -Ofast -march=native -mtune=native -DBENCHMARK
$ time ./a.out 104729
prime: 104729, power: 1

real    0m0.761s
user    0m0.760s
sys     0m0.000s
$ echo "scale=4; 0.761 * 1.6" | bc
1.2176
```
