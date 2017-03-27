#pragma once

#include "real.hpp"
#include <complex>

namespace emath
{
	using complex = std::complex<real>;  // Supports complex*float etc - convenient

//	struct complex
//	{
//		emath::real re, im;
//
//		// Mirrors std::complex:
//		emath::real real() const { return re; }
//		emath::real imag() const { return im; }
//	};
}
