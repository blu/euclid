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

static bool isFactored(const VecFactor& factor, const Number numerator)
{
	for (VecFactor::const_iterator it = factor.begin(); it != factor.end(); ++it) {
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

	VecFactor factor;
	factor.reserve(1024);

	Number number = Number(temp);
	Number candidateFactor = 1;

	while (1 < number) {
		while (isFactored(factor, ++candidateFactor)) {}

		Number power = 0;
		while (true) {
			const Number quotient = number / candidateFactor;
			const Number remainder = number % candidateFactor;

			// if merely looking for factors, as soon as candidateFactor crosses the sqrt(number)
			// mark we should add current number to factors and break the main loop; not doing that
			// gives us a desired side-effect of enumerating all primes below a given prime number

#if BENCHMARK == 0 && PRINT_ALL == 0
			if (quotient < candidateFactor) {
				factor.push_back(Factor(number, 1));
				number = 1;
				break;
			}

#endif
			if (remainder)
				break;

			power++;
			number = quotient;
		}

		factor.push_back(Factor(candidateFactor, power));
	}

#if BENCHMARK
	if (factor.size())
		printf("prime: %u, power: %u\n", factor.back().prime, factor.back().power);

#else
	for (VecFactor::const_iterator it = factor.begin(); it != factor.end(); ++it) {

		const Number factor = it->prime;
		const Number power = it->power;

#if PRINT_ALL == 0
		if (power)
			printf("prime: %u, power: %u (%u)\n", factor, power, ipow(factor, power));

#else
		printf("prime: %u, power: %u\n", factor, power);

#endif
	}

#endif // BENCHMARK
	return EXIT_SUCCESS;
}
