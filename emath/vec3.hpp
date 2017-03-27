#pragma once

#include "fwd.hpp"
#include "math.hpp" // Floor etc

namespace emath {

static_assert(std::is_pod<Vec3>::value,                 "is_pod");
static_assert(std::is_standard_layout<Vec3>::value,     "is_standard_layout");
static_assert(std::is_trivial<Vec3>::value,             "is_trivial");
static_assert(std::is_trivially_copyable<Vec3>::value,  "is_trivially_copyable");

static_assert(std::is_pod<Vec3u8>::value,                 "is_pod");
static_assert(std::is_standard_layout<Vec3u8>::value,     "is_standard_layout");
static_assert(std::is_trivial<Vec3u8>::value,             "is_trivial");
static_assert(std::is_trivially_copyable<Vec3u8>::value,  "is_trivially_copyable");

// ------------------------------------------------

template<typename T, class Tag>
inline T length_sq(const Vec3T<T, Tag>& v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

template<typename T, class Tag>
inline T length(const Vec3T<T, Tag>& v)
{
	return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

// Returns length
template<typename T, class Tag>
inline T normalize(Vec3T<T, Tag>& v) {
	T l = length(v);
	if (l != 0) {
		v *= 1.0f / l;
	}
	return l;
}

template<typename T, class Tag>
bool is_normalized(const Vec3T<T, Tag>& v) {
	return equals<T>(length_sq(v), 1);
}

template<typename T, class Tag>
T dot(const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<typename T, class Tag>
Vec3T<T, Tag> cross(const Vec3T<T, Tag>& lhs, const Vec3T<T, Tag>& rhs) {
	return {
		lhs[1]*rhs[2] - lhs[2]*rhs[1],
		lhs[2]*rhs[0] - lhs[0]*rhs[2],
		lhs[0]*rhs[1] - lhs[1]*rhs[0]
	};
}

// Scalar multiplication
template<typename T, class Tag>
Vec3T<T, Tag> mul(const Vec3T<T, Tag>& lhs, const Vec3T<T, Tag>& rhs)
{
	return { lhs[0]*rhs[0], lhs[1]*rhs[1], lhs[2]*rhs[2] };
}

template<typename T, class Tag>
Vec3T<T, Tag> div(const Vec3T<T, Tag>& lhs, const Vec3T<T, Tag>& rhs) {
	return {
		lhs[0] / rhs[0],
		lhs[1] / rhs[1],
		lhs[2] / rhs[2]
	};
}

template<typename T, class Tag>
T distance(const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b) {
	return length(a-b);
}

template<typename T, class Tag>
T distance_sq(const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b) {
	return length_sq(a-b);
}

// ------------------------------------------------

//	template<typename T>
//	inline std::ostream& operator<<(std::ostream& os, const Vec3T<T>& v)
//	{
//		return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
//	}

template<typename T, class Tag>
inline Vec3T<T, Tag> operator * (const Vec3T<T, Tag>& v, T s) { return {v.x*s, v.y*s, v.z*s}; }

template<typename T, class Tag>
inline Vec3T<T, Tag> operator * (T s, const Vec3T<T, Tag>& v) { return {v.x*s, v.y*s, v.z*s}; }

template<typename T, class Tag>
constexpr Vec3T<T, Tag> operator -(const Vec3T<T, Tag>& a) { return {-a.x, -a.y, -a.z}; }

template<typename T, class Tag>
constexpr const Vec3T<T, Tag>& operator +(const Vec3T<T, Tag>& a) { return a; }

template<typename T, class Tag>
constexpr Vec3T<T, Tag>  operator +  (const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template<typename T, class Tag>
inline Vec3T<T, Tag>& operator += (Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}

template<typename T, class Tag>
inline Vec3T<T, Tag>  operator -  (const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b)
{
	return {a.x-b.x, a.y-b.y, a.z-b.z};
}

template<typename T, class Tag>
inline Vec3T<T, Tag>& operator -= (Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return a;
}

template<typename T, class Tag>
inline Vec3T<T, Tag>& operator *= (Vec3T<T, Tag>& a, T v)
{
	a.x *= v;
	a.y *= v;
	a.z *= v;

	return a;
}

/*
template<typename T, class Tag>
inline Vec3T<T, Tag>  operator *  (const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& v)
{
	return {a.x*v.x, a.y*v.y, a.z*v.z};
}
*/

template<typename T, class Tag>
inline Vec3T<T, Tag>& operator *= (Vec3T<T, Tag>& a, const Vec3T<T, Tag>& v)
{
	a.x *= v.x;
	a.y *= v.y;
	a.z *= v.z;

	return a;
}

template<typename T, class Tag>
inline Vec3T<T, Tag>  operator /  (const Vec3T<T, Tag>& a, T v)
{
	return {a.x/v, a.y/v, a.z/v};
}

template<typename T, class Tag>
inline Vec3T<T, Tag>& operator /= (Vec3T<T, Tag>& a, T v)
{
	a.x /= v;
	a.y /= v;
	a.z /= v;

	return a;
}

template<typename T, class Tag>
inline bool operator == (const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

template<typename T, class Tag>
inline bool operator != (const Vec3T<T, Tag>& a, const Vec3T<T, Tag>& b)
{
	return !(a == b);
}

// ------------------------------------------------
// Utilities

inline Vec3 normalized(const Vec3& v)
{
	auto len = length(v);
	if (len == 0) {
		return Zero;
	} else {
		return v / len;
	}
}

// Safe: normalize 0, return zero.
inline Vec3 normalized_or_zero(const Vec3& v)
{
	auto len = length(v);
	if (is_zero(len)) {
		return Vec3(0);
	} else {
		return v / len;
	}
}

inline Vec3 round(Vec3 v)
{
	return Vec3(std::round(v.x), std::round(v.y), std::round(v.z));
}

inline Vec3i round_to_int(Vec3 v)
{
	return Vec3i(round_to_int(v.x), round_to_int(v.y), round_to_int(v.z));
}

inline Vec3i floor_to_int(Vec3 v)
{
	return Vec3i(floor_to_int(v.x), floor_to_int(v.y), floor_to_int(v.z));
}

inline Vec3i ceil_to_int(Vec3 v)
{
	return Vec3i(ceil_to_int(v.x), ceil_to_int(v.y), ceil_to_int(v.z));
}

inline Vec3i sign(Vec3 v)
{
	return Vec3i(sign(v.x), sign(v.y), sign(v.z));
}

// Works like in glsl
inline Vec3 reflect(const Vec3& d, const Vec3& n)
{
	return d - 2*dot(d, n)*n;
}

template<typename T, class Tag>
inline Vec3T<T,Tag> abs(const Vec3T<T,Tag>& v)
{
	return {std::abs(v.x), std::abs(v.y), std::abs(v.z)};
}

template<typename T, class Tag>
inline T max3(const Vec3T<T,Tag>& v)
{
	return max3(v.x, v.y, v.z);
}

// e.g. up is [0,0,1], we return things like [x,y,0]
inline Vec3 project_onto(Vec3 v, Vec3 up)
{
	return v - up * dot(v, up);
}
} // namespace emath

namespace std {
inline bool isfinite(emath::Vec3 v)
{
	return isfinite(v.x) && isfinite(v.y) && isfinite(v.z);
}
} // namespace std
