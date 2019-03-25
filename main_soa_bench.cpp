#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

typedef uint32_t Number;
typedef uint8_t Power;

typedef std::vector< Number > VecFactor;
typedef std::vector< Power > VecPower;

static bool isFactored(const VecFactor& factors, const Number numerator)
{
	for (VecFactor::const_iterator it = factors.begin(); it != factors.end(); ++it) {
		const Number prime_i = *it;
		const Number quotient = numerator / prime_i;
		const Number remainder = numerator % prime_i;

		if (quotient < prime_i)
			return false;

		if (0 == remainder)
			return true;
	}

	return false;
}

int main(int argc, char** argv)
{
	uint32_t temp;

	if (2 != argc || 1 != sscanf(argv[1], "%u", &temp)) {
		fprintf(stderr, "usage: %s positive-integer\n", argv[0]);
		return EXIT_FAILURE;
	}

	const size_t reserve = 1024;
	VecFactor factors;
	VecPower powers;
	factors.reserve(reserve);
	powers.reserve(reserve);

	Number number = Number(temp);
	Number candidateFactor = 1;

	while (1 < number) {
		while (isFactored(factors, ++candidateFactor)) {}

		Power power = 0;
		while (true) {
			const Number quotient = number / candidateFactor;
			const Number remainder = number % candidateFactor;

			// when looking for factors, as soon as candidateFactor crosses the sqrt(number) mark
			// we can add current number to factors and break the main loop; not doing that produces
			// a side-effect of enumerating all primes below a given prime number -- we use that here
			// for a benchmark

			if (remainder)
				break;

			power++;
			number = quotient;
		}

		factors.push_back(candidateFactor);
		powers.push_back(power);
	}

	if (factors.size())
		printf("prime: %u, power: %u\n", factors.back(), powers.back());

	return EXIT_SUCCESS;
}
