#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

typedef uint32_t Number;

struct Factor {
	Number prime;
	Number power;

	Factor(const Number prime, const Number power)
	: prime(prime)
	, power(power)
	{}
};

typedef std::vector< Factor > VecFactor;

static bool isFactored(const VecFactor& factors, const Number numerator)
{
	for (VecFactor::const_iterator it = factors.begin(); it != factors.end(); ++it) {
		const Number prime_i = it->prime;
		const Number quotient = numerator / prime_i;
		const Number remainder = numerator % prime_i;

		if (quotient < prime_i)
			return false;

		if (0 == remainder)
			return true;
	}

	return false;
}

static Number ipow(const Number base, Number power)
{
	Number res = 1;

	while (power--)
		res *= base;

	return res;
}

int main(int argc, char** argv)
{
	uint32_t temp;

	if (2 != argc || 1 != sscanf(argv[1], "%u", &temp)) {
		fprintf(stderr, "usage: %s positive-integer\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (1 == temp)
		return EXIT_SUCCESS;

	VecFactor factors;
	factors.reserve(1024);

	Number number = Number(temp);
	Number candidateFactor = 1;
	Number power = 0;

	// factorization by 2 is handled in advance
	while (true) {
		const Number quotient = number / 2;
		const Number remainder = number % 2;

		if (quotient < 2)
			break;

		if (remainder)
			break;

		power++;
		number = quotient;
	}

	if (number == 2) {
		factors.push_back(Factor(2, power + 1));
		goto printout;
	} else
		factors.push_back(Factor(2, power));

	while (true) {
		while (isFactored(factors, candidateFactor += 2)) {}

		power = 0;
		while (true) {
			const Number quotient = number / candidateFactor;
			const Number remainder = number % candidateFactor;

			// when looking for factors, as soon as candidateFactor crosses the sqrt(number) mark
			// we should add current number to factors and break the main loop; not doing that
			// produces a side-effect of enumerating all primes below a given prime number

			if (quotient < candidateFactor)
				goto main_loop_done;

			if (remainder)
				break;

			power++;
			number = quotient;
		}

		factors.push_back(Factor(candidateFactor, power));
	}

main_loop_done:

	if (number != candidateFactor) {
		factors.push_back(Factor(candidateFactor, power));
		factors.push_back(Factor(number, 1));
	}
	else
		factors.push_back(Factor(candidateFactor, power + 1));

printout:

	for (VecFactor::const_iterator it = factors.begin(); it != factors.end(); ++it) {
		const Number factor = it->prime;
		const Number power = it->power;

#if PRINT_ALL == 0
		if (power)
			printf("prime: %u, power: %u (%u)\n", factor, power, ipow(factor, power));

#else
		printf("prime: %u, power: %u\n", factor, power);

#endif
	}

	return EXIT_SUCCESS;
}
