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
	for (VecFactor::const_iterator it = factor.begin(); it != factor.end(); ++it)
		if (0 == numerator % it->prime)
			return true;

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
	long temp;

	if (2 != argc || 0 >= (temp = atol(argv[1]))) {
		fprintf(stderr, "usage: %s positive-integer\n", argv[0]);
		return EXIT_FAILURE;
	}

	VecFactor factor;
	factor.reserve(1024);

	Number number = Number(temp);
	Number candidateFactor = 2;

	while (1 < number) {
		while (isFactored(factor, candidateFactor)) {
			candidateFactor += 1;
		}

		Number power = 0;
		while (0 == number % candidateFactor) {
			power += 1;
			number /= candidateFactor;
		}

		factor.push_back(Factor(candidateFactor, power));
	}

#if BENCHMARK
	if (factor.size())
		printf("prime: %d, power: %d\n", factor.back().prime, factor.back().power);

#else
	for (VecFactor::const_iterator it = factor.begin(); it != factor.end(); ++it) {
		if (it->power)
			printf("prime: %d, power: %d (%d)\n", it->prime, it->power, ipow(it->prime, it->power));
		else
			printf("prime: %d, power: %d\n", it->prime, it->power);
	}

#endif
	return EXIT_SUCCESS;
}
