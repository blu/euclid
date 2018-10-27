#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

typedef uint32_t Number;
typedef uint8_t Power;

typedef std::vector< Number > VecFactor;
typedef std::vector< Power > VecPower;

// return a conservative estimate of the square root of the argument; estimate guaranteed not to be
// less than the real square root; being a stair-step function, routine tends to overshoot exact roots.
static Number irootApprox(const Number num)
{
	const size_t bitcount = sizeof(Number) * 8 - __builtin_clz(num);
	return (1 << (bitcount + 1) / 2) - 1;
}

static bool isFactored(const VecFactor& factors, const Number numerator)
{
	const Number sqrtNumerator = irootApprox(numerator);

	for (VecFactor::const_iterator it = factors.begin(); it != factors.end(); ++it) {
		const Number prime_i = *it;

		if (sqrtNumerator < prime_i)
			return false;

		if (0 == numerator % prime_i)
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
	uint32_t temp;

	if (2 != argc || 1 != sscanf(argv[1], "%u", &temp)) {
		fprintf(stderr, "usage: %s positive-integer\n", argv[0]);
		return EXIT_FAILURE;
	}

#if BENCHMARK
	const size_t reserve = 1 << 13;

#else
	const size_t reserve = 1 << 10;

#endif
	VecFactor factors;
	VecPower powers;
	factors.reserve(reserve);
	powers.reserve(reserve);

	Number number = Number(temp);
	Number candidateFactor = 1;
	Number sqrtNumber = irootApprox(number);

	while (1 < number) {
		while (isFactored(factors, ++candidateFactor)) {}

		// if merely looking for factors, as soon as candidateFactor crosses the sqrt(number)
		// mark we should add current number to factors and break this loop; not doing that
		// gives us a desired side-effect of enumerating all primes below a given prime number

#if BENCHMARK == 0 && PRINT_ALL == 0
		if (candidateFactor > sqrtNumber) {
			factors.push_back(number);
			powers.push_back(1);
			break;
		}

#endif
		Power power = 0;
		while (0 == number % candidateFactor) {
			power++;
			number /= candidateFactor;
			sqrtNumber = irootApprox(number);
		}

		factors.push_back(candidateFactor);
		powers.push_back(power);
	}

#if BENCHMARK
	if (factors.size())
		printf("prime: %d, power: %d\n", factors.back(), powers.back());

#else
	VecFactor::const_iterator itfactor = factors.begin();
	VecPower::const_iterator itpower = powers.begin();

	for (; itfactor != factors.end(); ++itfactor, ++itpower) {

		const Number factor = *itfactor;
		const Power power = *itpower;

#if PRINT_ALL == 0
		if (power)
			printf("prime: %d, power: %d (%d)\n", factor, power, ipow(factor, power));

#else
		printf("prime: %d, power: %d\n", factor, power);

#endif
	}

#endif // BENCHMARK
	return EXIT_SUCCESS;
}
