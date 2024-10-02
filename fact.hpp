#ifndef __fact_hpp
#define __fact_hpp

template < typename NUMBER_T >
struct Factor {
	NUMBER_T prime;
	NUMBER_T power;

	Factor () {}
	Factor(const NUMBER_T prime, const NUMBER_T power)
	: prime(prime)
	, power(power)
	{}
};

template < typename NUMBER_T >
class Storage {
	size_t size;
	Factor< NUMBER_T > *buf;

public:
	Storage(Factor< NUMBER_T > * const extbuf)
	: buf(extbuf)
	, size(0)
	{}

	void push_back(const NUMBER_T factor, const NUMBER_T power)
	{
		buf[size++] = Factor(factor, power);
	}

	void push_back_nonzero(const NUMBER_T factor, const NUMBER_T power)
	{
		if (power)
			push_back(factor, power);
	}

	const Factor< NUMBER_T > *begin() const
	{
		return buf;
	}

	const Factor< NUMBER_T > *end() const
	{
		return buf + size;
	}
};

#endif // __fact_hpp
