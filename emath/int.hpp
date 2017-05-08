// Version 1.1
#pragma once

#include <cstddef>

namespace emath {

// Returns the next power-of-two HIGHER OR EQUAL to k.
inline size_t next_power_of_two(size_t k)
{
	if (k==0) {
		return 1;
	}

	k--;
	for (size_t i=1; i<sizeof(size_t)*8; i=i*2) {
		k = (k | (k >> i));
	}
	return k+1;
}

inline constexpr bool is_power_of_two(size_t k)
{
	return (k & (k-1))==0;
}

/// Returns v if v%N==0
inline constexpr size_t next_multiple_of_n(size_t v, size_t N)
{
	return (v==0 ? 0 : ((v-1)/N + 1)*N);
}

/// Returns v if v%N==0
inline constexpr unsigned prev_multiple_of_n(unsigned v, unsigned N)
{
	return (v/N)*N;
}

inline constexpr bool is_multiple_of_n(unsigned v, unsigned N)
{
	return (v/N)*N == v;
}

/// Useful for hashing
constexpr unsigned HUGE_PRIME_0 = 0x8da6b343;
constexpr unsigned HUGE_PRIME_1 = 0xd8163841;
constexpr unsigned HUGE_PRIME_2 = 0xcb1ab31f;

inline int positive_modulo(int i, int n)
{
	return (i % n + n) % n;
}

} // namespace emath
