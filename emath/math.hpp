// Very old, Used in Ghostel.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint> // uint8_t etc
#include <limits>

#include "real.hpp"

namespace emath {
	#define IS_FLOATING_POINT(Type) (std::is_floating_point<Type>::value)
	#define STATIC_ASSERT_FLOATING_POINT(Type) static_assert(IS_FLOATING_POINT(Type), "Must be a float")

	template<class T>
	constexpr T PI = static_cast<T>(3.1415926535897932384626433);

	template<class T>
	constexpr T TAU = 2 * PI<T>;

	template<class T>
	constexpr T INF = std::numeric_limits<T>::infinity();

	constexpr float PIf  = PI<float>;
	constexpr float TAUf = 2 * PIf; ///< Oh yes. http://tauday.com/tau-manifesto.pdf
	constexpr float NaNf = std::numeric_limits<float>::quiet_NaN();
	constexpr float INFf = std::numeric_limits<float>::infinity();

	// ----------------------------------------------------------------------------

	template<typename F>
	constexpr int floor_to_int(F f)
	{
		return (int)std::floor(f); // TODO: doesn't work as constexpr!
	}

	template<typename F>
	constexpr int ceil_to_int(F f)
	{
		return (int)std::ceil(f);
	}

	/// nearest integer, rounding away from zero in halfway cases
	template<typename F>
	constexpr int round_to_int(F f)
	{
		//return (int)round(f); // doesn't work as constexpr!
		//return floor_to_int(f+0.5f);
		return int(f < 0 ? f-0.5f : f+0.5f); // int(foo) rounds towards zero
	}
	static_assert(round_to_int(+0.4) == +0, "round_to_int test");
	static_assert(round_to_int(+0.5) == +1, "round_to_int test");
	static_assert(round_to_int(+0.6) == +1, "round_to_int test");
	static_assert(round_to_int(-0.4) == +0, "round_to_int test");
	static_assert(round_to_int(-0.5) == -1, "round_to_int test");
	static_assert(round_to_int(-0.6) == -1, "round_to_int test");

	/// nearest integer, rounding away from zero in halfway cases
	template<typename F>
	constexpr unsigned round_to_uint(F f)
	{
		return unsigned(f + 0.5f);
	}
	static_assert(round_to_uint(+0.4) == +0, "round_to_uint test");
	static_assert(round_to_uint(+0.5) == +1, "round_to_uint test");
	static_assert(round_to_uint(+0.6) == +1, "round_to_uint test");

	// ----------------------------------------------------------------------------

	template<typename T>
	inline constexpr T clamp(T x, T min, T max)
	{
		return (x < min ? min : x > max ? max : x);
	}

	template<typename T>
	inline constexpr T saturate(T x)
	{
		return clamp<T>(x, 0, 1);
	}

	template<typename T>
	inline constexpr T lerp(const T& a, const T& b, float t)
	{
		return a*(1-t) + b*t;
	}

	template<typename T>
	inline constexpr T lerp(const T& a, const T& b, double t)
	{
		return a*(1-t) + b*t;
	}

	// For color-components:
	template<>
	inline constexpr uint8_t lerp(const uint8_t& a, const uint8_t& b, float t)
	{
		return (uint8_t)round_to_int((1 - t) * a + t * b);
	}

	inline float remap(float x, float in_min, float in_max,
	                   float out_min, float out_max)
	{
		float t = (x - in_min) / (in_max - in_min);
		return lerp(out_min, out_max, t);
	}

	inline float remap_clamp(float x, float in_min, float in_max,
	                         float out_min, float out_max)
	{
		float t = (x - in_min) / (in_max - in_min);
		t = saturate(t);
		return lerp(out_min, out_max, t);
	}

	/// Optional last argument with an easing, e.g. ease_in_ease_out
	inline float remap_clamp(float x, float in_min, float in_max,
	                         float out_min, float out_max, float(*ease)(float))
	{
		float t = (x - in_min) / (in_max - in_min);
		t = saturate(t);
		t = ease(t);
		return lerp(out_min, out_max, t);
	}

	// ----------------------------------------------------------------------------

	template<typename T>
	constexpr T min3(const T& a, const T& b, const T& c)
	{
		return std::min(a, std::min(b, c));
	}

	template<typename T>
	constexpr T max3(const T& a, const T& b, const T& c)
	{
		return std::max(a, std::max(b, c));
	}

	// ----------------------------------------------------------------------------

	template<typename T>
	inline constexpr T sqr(T x)
	{
		return x*x;
	}

	template<typename T>
	inline constexpr T cube(T x)
	{
		return x*x*x;
	}

	// ----------------------------------------------------------------------------

	inline float deg2rad(float a)
	{
		return a * PIf / 180;
	}

	inline float rad2deg(float a)
	{
		return a * 180 / PIf;
	}

	// ----------------------------------------------------------------------------

	// For t=[0,1], returns [0,1] with a derivate of zero at both ends
	template<typename T>
	constexpr T ease_in_ease_out(T t)
	{
		return 3*t*t - 2*t*t*t;
	}

	// ------------------------------------------------
	// Numbers smaller than eps are easily rounding errors

	template<typename F>
	inline constexpr F Eps();

	template<>
	inline constexpr float Eps() { return 2e-5f; }

	template<>
	inline constexpr double Eps() { return 1e-11; } // Way more than DBL_EPSILON... but whatever.

	const double EPSd = Eps<double>();
	const float  EPSf = Eps<float>();

	// ------------------------------------------------

	template<typename F>
	constexpr F abs(F f)
	{
		return (f<0 ? -f : f);
	}
	static_assert(emath::abs(-.1f) == +.1f, "emath::abs");

	template<typename T>
	inline constexpr int sign(const T& val)
	{
		return (val<0 ? -1 : val>0 ? +1 : 0);
	}

	template<typename T>
	inline constexpr T signF(const T& val)
	{
		return (val<0 ? (T)-1 : val>0 ? (T)+1 : (T)0);
	}

	template<typename T>
	constexpr auto average(const T& a, const T& b)
	{
		return (a + b)/2;
	}

	// To [-PI, +PI]
	template<typename T>
	inline T wrap_angle(T a)
	{
		while (a < -PIf) { a += TAU<T>; }
		while (a > +PIf) { a -= TAU<T>; }
		return a;
	}

	inline real lerp_angle(real a0, real a1, float t)
	{
		return a0 + t * wrap_angle(a1-a0);
	}

	template<typename T>
	inline void sort(T& a, T& b)
	{
		if (b<a) { std::swap(a,b); }
	}

	inline constexpr bool is_zero(real v, real eps = EPSf)
	{
		return abs(v) <= eps;
	}

	template<typename T>
	inline constexpr T min(T a, T b)
	{
		return a<b ? a : b;
	}

	template<typename T>
	inline constexpr T max(T a, T b)
	{
		return b<a ? a : b;
	}

	template<typename T>
	inline constexpr bool equals(T a, T b, T eps = Eps<T>())
	{
#if 0
		bad
		return abs(a-b) <= eps * max(abs(a),abs(b));
#elif 0
		return
		//std::isnan(a) || std::isnan(b) ? false :
		//std::isinf(a) && std::isinf(b) ? sign(a) == sign(b) :
		//std::isinf(a) || std::isinf(b) ? false :
		a*b==0 ? abs(a+b) <= eps :               // Any zero?
		a*b<0  ? abs(a-b) <= eps :               // Different signs?
		abs(a-b) <= eps * max(abs(a),abs(b));
#else
		return abs(a-b) < eps;
#endif
	}

	//static_assert(!equals<float>(+NaN, +NaN),  "emath::equals broken");
	//static_assert( equals<float>(+INF, +INF),             "emath::equals broken");
	//static_assert(!equals<float>(+INF, -INF),             "emath::equals broken");
	static_assert( equals<float>(+1, +1),                   "emath::equals broken");
	static_assert( equals<float>(-1, -1),                   "emath::equals broken");
	static_assert(!equals<float>(-1, +1),                   "emath::equals broken");
	static_assert( equals<float>(+1, +1.000001, 1e-5),      "emath::equals broken");
	static_assert(!equals<float>(+1, +1.0001, 1e-5),        "emath::equals broken");
	//static_assert( equals<float>(+1000000, +1000001, 1e-5),        "emath::equals broken"); // TODO

	/*
	 Interpolate the cubic Hermite spline from:
	 point p0 with tangent m0 at t=0.
	 to
	 point p1 with tangent m1 at t=1.
	 */
	template<typename T>
	inline T hermite(T p0, T m0, T p1, T m1, float t)
	{
		float t2 = t*t;
		float t3 = t2*t;
		return (2*t3 - 3*t2 + 1)*p0 + (t3 - 2*t2 + t)*m0 + (-2*t3 + 3*t2)*p1 + (t3 - t2)*m1;
	}

	// For t=[0,1], returns [0,1] with a derivate of zero at both ends
	template<typename T>
	constexpr T ease_in_ease_out(T t)
	{
		return 3*t*t - 2*t*t*t;
	}

	// normalized sinc function
	template<typename F>
	inline F sinc(F x)
	{
		STATIC_ASSERT_FLOATING_POINT(F);
		if (x==0) return 1; // Prevent singularity
		return std::sin(PI<F> * x)/(PI<F> * x);
	}

	// t is [0,1] between p1 and p2
	template<typename F, typename T>
	inline T catmull_rom(F t, T p0, T p1, T p2, T p3)
	{
		STATIC_ASSERT_FLOATING_POINT(F);
		return 0.5f * (
							p0 * t*((2-t)*t-1)   +
							p1 * (t*t*(3*t-5)+2) +
							p2 * t*((4-3*t)*t+1) +
							p3 * (t-1)*t*t
							);
	}

	template<typename F, typename T>
	inline T catmull_rom(F t, T points[4])
	{
		return catmull_rom(t, points[0], points[1], points[2], points[3]);
	}

	// ------------------------------------------------

	// Returns the next float greater than 'arg'.
	// if NAN, will return NAN.
	// if INF, will return INF.
	// if -INF, will return std::numeric_limit<float>::min()
	// Will allow denormal numbers.
	inline float next_float(float arg);

	// ------------------------------------------------

	// Easily spotted value at debug time
	template<typename T>
	inline const T get_debug_value() { return T(); }

	template<>
	inline const float get_debug_value() { return std::numeric_limits<float>::signaling_NaN(); }

	template<>
	inline const double get_debug_value() { return std::numeric_limits<double>::signaling_NaN(); }

	template<>
	inline const int get_debug_value() { return -1; }
}
