#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

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

int main(int argc, char** argv)
{
	uint64_t temp;

	if (2 != argc || 1 != sscanf(argv[1], FMT64, &temp)) {
		fprintf(stderr, "usage: %s positive-integer\n", argv[0]);
		return EXIT_FAILURE;
	}

	VecFactor factors;
	factors.reserve(1024);

	Number number = Number(temp);
	Number candidateFactor = 1;

	while (1 < number) {
		while (isFactored(factors, ++candidateFactor)) {}

		Number power = 0;
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

		factors.push_back(Factor(candidateFactor, power));
	}

	if (factors.size())
		printf("prime: " NUM_FMT ", power: " NUM_FMT "\n", factors.back().prime, factors.back().power);

	return EXIT_SUCCESS;
}
