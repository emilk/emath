// Version 1.1
#pragma once

#include <cassert>

#include "fwd.hpp"
#include "int.hpp" // is_power_of_two
#include "math.hpp"

#if __APPLE__
#   include <CoreGraphics/CGGeometry.h> // CGPoint & CGSize
#endif

#ifdef __clang__
	// warning: anonymous structs are a GNU extension [-pedantic,-Wgnu]
	//#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-Wgnu"
#else
	#pragma GCC diagnostic ignored "-pedantic"
#endif

namespace emath {

// ------------------------------------------------
// Static constructors:

/// Given a unit-vector, if the angle is VERY close to being a factor of 45° the the normal will be modified to be perfect.
template<typename T>
const Vec2T<T> idealized_normal(Vec2T<T> vec)
{
	if (is_zero(emath::abs(vec.x) - emath::abs(vec.y))) {
		// N * 45 deg
		vec.x = signF(vec.x);
		vec.y = signF(vec.y);
		vec *= std::sqrt((T)2) / 2;
	} else {
		for (unsigned a=0; a<2; ++a) {
			if (is_zero(emath::abs(vec[a]))) {
				vec[ a ] = 0;
				vec[1-a] = (T)sign(vec[1-a]);
				break;
			}
		}
	}
	return vec;
}

// Returns the unit-vector of a certain angle (angle=0, vector=[1,0], angle=Pi/2, vector=[0,1])
// The invert of Vec2T::angle()
template<typename T>
inline const Vec2T<T> vec2_angled(T a)
{
	auto ret = Vec2T<T>(std::cos(a), std::sin(a));
	ret = idealized_normal(ret);
	return ret;
}

// ------------------------------------------------

inline float length_sq(const Vec2f& v)
{
	return v.x*v.x + v.y*v.y;
}

inline float sq(const Vec2f& v)
{
	return v.x*v.x + v.y*v.y;
}

inline float length(const Vec2f& v)
{
	return std::sqrt(v.x*v.x + v.y*v.y);
}

inline float area(const Vec2f& v)
{
	return v.x * v.y;
}

// The angle of the vector. angle(Vec2f)(1,0)) == 0 and angle(Vec2f(0,1)) == Pi/2
// Returns an angle in [-pi, +pi]
// The invert of vec2_angled
inline float angle(const Vec2f& v)
{
	if (v.x == 0 && v.y == 0) {
		return 0;
	}
	return std::atan2(v.y, v.x);
}

// Returns length
template<typename T>
inline T normalize(Vec2T<T>& v)
{
	T l = length(v);
	if (l != 0) {
		v *= 1.0f/l;
	}
	return l;
}

template<typename T>
inline T area(const Vec2T<T>& v)
{
	return v.x * v.y;
}

// ------------------------------------------------
// Operators

template<typename T>
T  Vec2T<T>::operator[](int i) const { assert(0<=i && i<2); return data()[i]; }

template<typename T>
T& Vec2T<T>::operator[](int i)       { assert(0<=i && i<2); return data()[i]; }

template<typename T>
T  Vec2T<T>::operator[](unsigned i) const { assert(0<=i && i<2); return data()[i]; }

template<typename T>
T& Vec2T<T>::operator[](unsigned i)       { assert(0<=i && i<2); return data()[i]; }

template<typename T>
constexpr Vec2T<T> operator-(const Vec2T<T>& v) {
	return {-v.x, -v.y};
}

template<typename T>
constexpr const Vec2T<T>& operator+(const Vec2T<T>& v)
{
	return v;
}

// ------------------------------------------------

template<typename T>
Vec2T<T>& operator+=(Vec2T<T>& a, const Vec2T<T>& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

template<typename T>
Vec2T<T>& operator-=(Vec2T<T>& a, const Vec2T<T>& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

template<typename T>
Vec2T<T>& operator*=(Vec2T<T>& a, T s)
{
	a.x *= s;
	a.y *= s;
	return a;
}

// ------------------------------------------------

template<typename T>
constexpr const Vec2T<T> operator+(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return Vec2T<T>(a.x + b.x, a.y + b.y);
}

template<typename T>
constexpr const Vec2T<T> operator-(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return Vec2T<T>(a.x - b.x, a.y - b.y);
}

template<typename T>
constexpr auto operator*(T s, const Vec2T<T>& a) -> Vec2T<decltype(s*a.x)>
{
	return {s * a.x, s * a.y};
}

template<typename T, typename S>
constexpr auto operator*(const Vec2T<T>& a, S s) -> Vec2T<decltype(a.x*s)>
{
	return {a.x * s, a.y * s};
}

#if 1
template<typename T, typename S>
constexpr auto operator/(const Vec2T<T>& a, S s) -> Vec2T<decltype(a.x/s)>
{
	return {a.x / s, a.y / s};
}
#else
constexpr Vec2f operator/(const Vec2f& a, float s)
{
	return {a.x/s, a.y/s};
}
#endif
// ------------------------------------------------

template<typename T>
constexpr bool operator==(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return a.x==b.x && a.y==b.y;
}

template<typename T>
constexpr bool operator!=(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return a.x!=b.x || a.y!=b.y;
}

template<typename T>
constexpr bool operator<(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return (a.x!=b.x ? a.x<b.x : a.y<b.y);
}

// ------------------------------------------------

template<typename T>
T dot(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return a.x*b.x + a.y*b.y;
}

template<typename T>
T cross(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return a.x * b.y - a.y * b.x;
}

template<typename T>
Vec2T<T> cross(const Vec2T<T>& a, T z)
{
	return {z*a.y, -z*a.x};
}

template<typename T>
Vec2T<T> cross(T z, const Vec2T<T>& a)
{
	return {-z*a.y, z*a.x};
}

// ------------------------------------------------

template<typename T>
T distance(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return length(a-b);
}

template<typename T>
T distance_sq(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return length_sq(a-b);
}

// ------------------------------------------------
// utils

template<typename T>
inline const Vec2T<T> rot90CCW(const Vec2T<T>& v)
{
	return {-v.y, v.x};
}

template<typename T>
inline const Vec2T<T> rot90CW(const Vec2T<T>& v)
{
	return {v.y, -v.x};
}

// ------------------------------------------------

inline Vec2f normalized(const Vec2f& v)
{
	auto len = length(v);
	if (len == 0) {
		return {0,0};
	} else {
		return v / len;
	}
}

// Safe: normalize 0, return zero.
inline Vec2f normalized_or_zero(const Vec2f& v)
{
	auto len = length(v);
	if (is_zero(len)) {
		return {0,0};
	} else {
		return v / len;
	}
}

/// The difference in angle between two vectors b and a.
inline float vec2_angle_diff(const Vec2f& b, const Vec2f& a)
{
	return angle(b) - angle(a); // TODO: optimize
}

template<typename T>
inline Vec2T<T> div(T t, const Vec2T<T>& vec)
{
	return {t/vec.x, t/vec.y};
}

template<typename T>
inline Vec2T<T> div(Vec2T<T> a, const Vec2T<T>& b)
{
	return {a.x/b.x, a.y/b.y};
}

template<typename T>
inline Vec2T<T> mul(Vec2T<T> a, const Vec2T<T>& b)
{
	return {a.x*b.x, a.y*b.y};
}

// Works like in glsl
inline Vec2f reflect(const Vec2f& d, const Vec2f& n)
{
	return d - 2 * dot(d, n) * n;
}

// ------------------------------------------------

inline bool is_power_of_two(Vec2u vec)
{
	return emath::is_power_of_two(vec.x) && emath::is_power_of_two(vec.y);
}

inline Vec2f round(Vec2f v)
{
	return {std::round(v.x), std::round(v.y)};
}

inline Vec2i round_to_int(Vec2f v)
{
	return {round_to_int(v.x), round_to_int(v.y)};
}

inline Vec2i floor_to_int(Vec2f v)
{
	return {floor_to_int(v.x), floor_to_int(v.y)};
}

inline Vec2i ceil_to_int(Vec2f v)
{
	return {ceil_to_int(v.x), ceil_to_int(v.y)};
}

inline Vec2i sign(Vec2f v)
{
	return {sign(v.x), sign(v.y)};
}

template<typename T>
inline Vec2T<T> abs(Vec2T<T> v)
{
	return {abs(v.x), abs(v.y)};
}

template<typename T>
T min(const Vec2T<T>& v)
{
	return emath::min(v.x, v.y);
}

template<typename T>
T max(const Vec2T<T>& v)
{
	return emath::max(v.x, v.y);
}

template<typename T>
Vec2T<T> min(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return {emath::min(a.x, b.x), emath::min(a.y, b.y)};
}

template<typename T>
Vec2T<T> max(const Vec2T<T>& a, const Vec2T<T>& b)
{
	return {emath::max(a.x, b.x), emath::max(a.y, b.y)};
}

#if 1
// ensure [0,size)
inline Vec2i clamp_to_size(Vec2i p, Vec2i size)
{
	return {min(size-Vec2i(1), max(Vec2i(0), p))};
}
#endif

template<typename T>
inline Vec2T<T> clamp(Vec2T<T> v, Vec2T<T> mn, Vec2T<T> mx)
{
	Vec2T<T> ret;
	for (int d=0; d<2; ++d) {
		ret[d] = emath::clamp(v[d], mn[d], mx[d]);
	}
	return ret;
}

template<typename T>
inline constexpr T sqr(Vec2T<T> v)
{
	return sq(v);
}

template<typename T>
inline T aspect_ratio(Vec2T<T> v)
{
	return (float)v.x / (float)v.y;
}

inline Vec2f fit_into(Vec2f size, Vec2f container)
{
	if (aspect_ratio(size) > aspect_ratio(container)) {
		return size * container.x / size.x;
	} else {
		return size * container.y / size.y;
	}
}

// ------------------------------------------------

inline Vec2f remap(Vec2f v, Vec2f in_min, Vec2f in_max, Vec2f out_min, Vec2f out_max)
{
	Vec2f ret;
	ret.x = remap(v.x, in_min.x, in_max.x, out_min.x, out_max.x);
	ret.y = remap(v.y, in_min.y, in_max.y, out_min.y, out_max.y);
	return ret;
}

inline Vec2f remap_clamp(Vec2f v, Vec2f in_min, Vec2f in_max, Vec2f out_min, Vec2f out_max)
{
	Vec2f ret;
	ret.x = remap_clamp(v.x, in_min.x, in_max.x, out_min.x, out_max.x);
	ret.y = remap_clamp(v.y, in_min.y, in_max.y, out_min.y, out_max.y);
	return ret;
}

	// ------------------------------------------------

//#if 1
//	template<typename T>
//	inline std::ostream& operator<<(std::ostream& os, const Vec2T<T>& v)
//	{
//		return os << "<" << v.x << ", " << v.y << ">";
//	}
//#endif
} // namespace emath

namespace std {
inline bool isfinite(emath::Vec2f v)
{
	return isfinite(v.x) && isfinite(v.y);
}
} // namespace std
