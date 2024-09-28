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

static Number ipow(const Number base, Number power)
{
	Number res = 1;

	while (power--)
		res *= base;

	return res;
}

Number count_trailing_zeroes(const Number x)
{
	if (sizeof(unsigned int) >= sizeof(x))
		return __builtin_ctz(x);

	if (sizeof(unsigned long) >= sizeof(x))
		return __builtin_ctzl(x);

	return __builtin_ctzll(x);
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

	VecFactor factors;
	factors.reserve(1024);

	Number number = Number(temp);
	Number candidateFactor = 1;
	Number power = 0;

	// factorization by 2 is handled in advance
	power = count_trailing_zeroes(number);
	number >>= power;

	if (power)
		factors.push_back(Factor(2, power));

	if (number == 1)
		goto printout;

	while (true) {
		candidateFactor += 2;

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

		if (power)
			factors.push_back(Factor(candidateFactor, power));
	}

main_loop_done:

	if (number != candidateFactor) {
		if (power)
			factors.push_back(Factor(candidateFactor, power));
		factors.push_back(Factor(number, 1));
	}
	else
		factors.push_back(Factor(candidateFactor, power + 1));

printout:

	for (VecFactor::const_iterator it = factors.begin(); it != factors.end(); ++it) {
		const Number factor = it->prime;
		const Number power = it->power;

		printf("prime: " NUM_FMT ", power: " NUM_FMT " (" NUM_FMT ")\n", factor, power, ipow(factor, power));
	}

	return EXIT_SUCCESS;
}
