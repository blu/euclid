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

static Number ipow(const Number base, Power power)
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

	const size_t reserve = 1024;
	VecFactor factors;
	VecPower powers;
	factors.reserve(reserve);
	powers.reserve(reserve);

	Number number = Number(temp);
	Number candidateFactor = 1;
	Power power = 0;

	while (true) {
		while (isFactored(factors, ++candidateFactor)) {}

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

		factors.push_back(candidateFactor);
		powers.push_back(power);
	}

main_loop_done:

	if (number != candidateFactor) {
		factors.push_back(candidateFactor);
		factors.push_back(number);
		powers.push_back(power);
		powers.push_back(1);
	}
	else {
		factors.push_back(candidateFactor);
		powers.push_back(power + 1);
	}

	VecFactor::const_iterator itfactor = factors.begin();
	VecPower::const_iterator itpower = powers.begin();

	for (; itfactor != factors.end(); ++itfactor, ++itpower) {
		const Number factor = *itfactor;
		const Power power = *itpower;

#if PRINT_ALL == 0
		if (power)
			printf("prime: " NUM_FMT ", power: %u (" NUM_FMT ")\n", factor, power, ipow(factor, power));

#else
		printf("prime: " NUM_FMT ", power: " NUM_FMT "\n", factor, power);

#endif
	}

	return EXIT_SUCCESS;
}
