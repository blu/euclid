#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fact.hpp"

#if _MSC_VER || __APPLE__ || __SIZEOF_LONG__ == 4
#define FMT64 "%llu"
#else
#define FMT64 "%lu"
#endif
#define FMT32 "%u"

#if NUM_64BIT
typedef uint64_t Number;
#define NUM_FMT FMT64
#else
typedef uint32_t Number;
#define NUM_FMT FMT32
#endif

// use scalar bitness as a conservative estimate of the limit of unique prime factors
Factor< Number > buf[sizeof(Number) * 8];

static Number ipow(const Number base, Number power)
{
	Number res = 1;

	while (power--)
		res *= base;

	return res;
}

int main(int argc, char** argv)
{
	uint64_t temp;

	if (2 != argc || 1 != sscanf(argv[1], FMT64, &temp)) {
		fprintf(stderr, "usage: %s positive-integer\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (1 == temp)
		return EXIT_SUCCESS;

	Storage factors(buf);

	Number number = Number(temp);
	Number candidateFactor = 1;
	Number power = 0;

	while (true) {
		candidateFactor++;
		power = 0;

		while (true) {
			const Number quotient = number / candidateFactor;
			const Number remainder = number % candidateFactor;

			// when looking for factors, as soon as candidateFactor crosses the sqrt(number) mark
			// we should add current number to factors and break the main loop

			if (quotient < candidateFactor)
				goto main_loop_done;

			if (remainder)
				break;

			power++;
			number = quotient;
		}

		factors.push_back_nonzero(candidateFactor, power);
	}

main_loop_done:

	if (number != candidateFactor) {
		factors.push_back_nonzero(candidateFactor, power);
		factors.push_back(number, 1);
	}
	else
		factors.push_back(candidateFactor, power + 1);

	for (const Factor< Number > *it = factors.begin(); it != factors.end(); ++it) {
		const Number factor = it->prime;
		const Number power = it->power;

		printf("prime: " NUM_FMT ", power: " NUM_FMT " (" NUM_FMT ")\n", factor, power, ipow(factor, power));
	}

	return EXIT_SUCCESS;
}
