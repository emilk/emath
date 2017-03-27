#pragma once

#include <cmath>
/*
	Code adapted from https://github.com/warrenm/AHEasing
	See http://easings.net/ for examples
*/

namespace easing
{
	// Christophe Schlick’s approximation of Ken Perlin's bias function
	// t=[0,1]. b=(0,1). b = f(0.5). b = 0.5 is linear.
	inline float schlick(float x, float b)
	{
		return x / ((1 / b - 2) * (1 - x) + 1);
	}

	// ------------------------------------------------------------------------

	// Modeled after the line y = x
	inline float linear_interpolation(float p)
	{
		return p;
	}

	// Modeled after the parabola y = x^2
	inline float quadratic_ease_in(float p)
	{
		return p * p;
	}

	// Modeled after the parabola y = -x^2 + 2x
	inline float quadratic_ease_out(float p)
	{
		return -(p * (p - 2));
	}

	// Modeled after the piecewise quadratic
	// y = (1/2)((2x)^2)             ; [0, 0.5f)
	// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5f, 1]
	inline float quadratic_ease_in_out(float p)
	{
		if (p < 0.5f) {
			return 2 * p * p;
		} else {
			return (-2 * p * p) + (4 * p) - 1;
		}
	}

	// Modeled after the cubic y = x^3
	inline float cubic_ease_in(float p)
	{
		return p * p * p;
	}

	// Modeled after the cubic y = (x - 1)^3 + 1
	inline float cubic_ease_out(float p)
	{
		float f = (p - 1);
		return f * f * f + 1;
	}

	// Modeled after the piecewise cubic
	// y = (1/2)((2x)^3)       ; [0, 0.5f)
	// y = (1/2)((2x-2)^3 + 2) ; [0.5f, 1]
	inline float cubic_ease_in_out(float p)
	{
		if (p < 0.5f) {
			return 4 * p * p * p;
		} else {
			float f = ((2 * p) - 2);
			return 0.5f * f * f * f + 1;
		}
	}

	// Modeled after the quartic x^4
	inline float quartic_ease_in(float p)
	{
		return p * p * p * p;
	}

	// Modeled after the quartic y = 1 - (x - 1)^4
	inline float quartic_ease_out(float p)
	{
		float f = (p - 1);
		return f * f * f * (1 - p) + 1;
	}

	// Modeled after the piecewise quartic
	// y = (1/2)((2x)^4)        ; [0, 0.5f)
	// y = -(1/2)((2x-2)^4 - 2) ; [0.5f, 1]
	inline float quartic_ease_in_out(float p)
	{
		if (p < 0.5f) {
			return 8 * p * p * p * p;
		} else {
			float f = (p - 1);
			return -8 * f * f * f * f + 1;
		}
	}

	// Modeled after the quintic y = x^5
	inline float quintic_ease_in(float p)
	{
		return p * p * p * p * p;
	}

	// Modeled after the quintic y = (x - 1)^5 + 1
	inline float quintic_ease_out(float p)
	{
		float f = (p - 1);
		return f * f * f * f * f + 1;
	}

	// Modeled after the piecewise quintic
	// y = (1/2)((2x)^5)       ; [0, 0.5f)
	// y = (1/2)((2x-2)^5 + 2) ; [0.5f, 1]
	inline float quintic_ease_in_out(float p)
	{
		if (p < 0.5f) {
			return 16 * p * p * p * p * p;
		} else {
			float f = ((2 * p) - 2);
			return  0.5f * f * f * f * f * f + 1;
		}
	}

	// Modeled after quarter-cycle of sine wave
	inline float sine_ease_in(float p)
	{
		return sinf((p - 1) * (float)M_PI_2) + 1;
	}

	// Modeled after quarter-cycle of sine wave (different phase)
	inline float sine_ease_out(float p)
	{
		return sinf(p * (float)M_PI_2);
	}

	// Modeled after half sine wave
	inline float sine_ease_in_out(float p)
	{
		return 0.5f * (1 - cosf(p * (float)M_PI));
	}

	// Modeled after shifted quadrant IV of unit circle
	inline float circular_ease_in(float p)
	{
		return 1 - sqrtf(1 - (p * p));
	}

	// Modeled after shifted quadrant II of unit circle
	inline float circular_ease_out(float p)
	{
		return sqrtf((2 - p) * p);
	}

	// Modeled after the piecewise circular function
	// y = (1/2)(1 - sqrtf(1 - 4x^2))           ; [0, 0.5f)
	// y = (1/2)(sqrtf(-(2x - 3)*(2x - 1)) + 1) ; [0.5f, 1]
	inline float circular_ease_in_out(float p)
	{
		if (p < 0.5f) {
			return 0.5f * (1 - sqrtf(1 - 4 * (p * p)));
		} else {
			return 0.5f * (sqrtf(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
		}
	}

	// Modeled after the exponential function y = 2^(10(x - 1))
	inline float exponential_ease_in(float p)
	{
		return (p == 0.0f) ? p : powf(2, 10 * (p - 1));
	}

	// Modeled after the exponential function y = -2^(-10x) + 1
	inline float exponential_ease_out(float p)
	{
		return (p == 1.0f) ? p : 1 - powf(2, -10 * p);
	}

	// Modeled after the piecewise exponential
	// y = (1/2)2^(10(2x - 1))         ; [0,0.5f)
	// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5f,1]
	inline float exponential_ease_in_out(float p)
	{
		if (p == 0.0f || p == 1.0f) return p;
		if (p < 0.5f) {
			return 0.5f * powf(2, (20 * p) - 10);
		} else {
			return -0.5f * powf(2, (-20 * p) + 10) + 1;
		}
	}

	// Modeled after the damped sine wave y = sinf(13pi/2*x)*powf(2, 10 * (x - 1))
	inline float elastic_ease_in(float p) // Not bounded in [0,1]!
	{
		return sinf(13 * (float)M_PI_2 * p) * powf(2, 10 * (p - 1));
	}

	// Modeled after the damped sine wave y = sinf(-13pi/2*(x + 1))*powf(2, -10x) + 1
	inline float elastic_ease_out(float p) // Not bounded in [0,1]!
	{
		return sinf(-13 * (float)M_PI_2 * (p + 1)) * powf(2, -10 * p) + 1;
	}

	// Modeled after the piecewise exponentially-damped sine wave:
	// y = (1/2)*sinf(13pi/2*(2*x))*powf(2, 10 * ((2*x) - 1))      ; [0,0.5f)
	// y = (1/2)*(sinf(-13pi/2*((2x-1)+1))*powf(2,-10(2*x-1)) + 2) ; [0.5f, 1]
	inline float elastic_ease_in_out(float p) // Not bounded in [0,1]!
	{
		if (p < 0.5f) {
			return 0.5f * sinf(13 * (float)M_PI_2 * (2 * p)) * powf(2, 10 * ((2 * p) - 1));
		} else {
			return 0.5f * (sinf(-13 * (float)M_PI_2 * ((2 * p - 1) + 1)) * powf(2, -10 * (2 * p - 1)) + 2);
		}
	}

	// Modeled after the overshooting cubic y = x^3-x*sinf(x*pi)
	inline float back_ease_in(float p) // Not bounded in [0,1]!
	{
		return p * p * p - p * sinf(p * (float)M_PI);
	}

	// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sinf((1-x)*pi))
	inline float back_ease_out(float p) // Not bounded in [0,1]!
	{
		float f = (1 - p);
		return 1 - (f * f * f - f * sinf(f * (float)M_PI));
	}

	// Modeled after the piecewise overshooting cubic function:
	// y = (1/2)*((2x)^3-(2x)*sinf(2*x*pi))           ; [0, 0.5f)
	// y = (1/2)*(1-((1-x)^3-(1-x)*sinf((1-x)*pi))+1) ; [0.5f, 1]
	inline float back_ease_in_out(float p) // Not bounded in [0,1]!
	{
		if (p < 0.5f) {
			float f = 2 * p;
			return 0.5f * (f * f * f - f * sinf(f * (float)M_PI));
		} else {
			float f = (1 - (2*p - 1));
			return 0.5f * (1 - (f * f * f - f * sinf(f * (float)M_PI))) + 0.5f;
		}
	}

	inline float bounce_ease_out(float p)
	{
		if (p < 4/11.0f) {
			return (121 * p * p)/16.0f;
		} else if (p < 8/11.0f) {
			return (363/40.0f * p * p) - (99/10.0f * p) + 17/5.0f;
		} else if (p < 9/10.0f) {
			return (4356/361.0f * p * p) - (35442/1805.0f * p) + 16061/1805.0f;
		} else {
			return (54/5.0f * p * p) - (513/25.0f * p) + 268/25.0f;
		}
	}

	inline float bounce_ease_in(float p)
	{
		return 1 - bounce_ease_out(1 - p);
	}

	inline float bounce_ease_in_out(float p)
	{
		if (p < 0.5f) {
			return 0.5f * bounce_ease_in(p*2);
		} else {
			return 0.5f * bounce_ease_out(p * 2 - 1) + 0.5f;
		}
	}
}
