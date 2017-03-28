#pragma once

#include "fwd.hpp"

namespace emath {

// ------------------------------------------------
// Definitions

//	template<typename T, class Tag>
//	inline std::ostream& operator<<(std::ostream& os, const Vec4T<T, Tag>& v)
//	{
//		return os << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
//	}

template<typename T, class Tag>
inline Vec4T<T, Tag>::Vec4T(T v) : x(v), y(v), z(v), w(v)
{
}

/*
template<typename T, class Tag>
inline Vec4T<T, Tag>::Vec4T(const Vec4T& v)
:x(v.x), y(v.y), z(v.z), w(v.w)
{
}

template<typename T, class Tag>
inline Vec4T<T, Tag>& operator =  (const Vec4T<T, Tag>& a, const Vec4T<T, Tag>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;

	return *this;
}
 */

template<typename T, class Tag>
constexpr Vec4T<T, Tag> operator+(const Vec4T<T, Tag>& v) {
	return {+v.x, +v.y, +v.z, +v.w};
}

template<typename T, class Tag>
constexpr Vec4T<T, Tag> operator-(const Vec4T<T, Tag>& v) {
	return {-v.x, -v.y, -v.z, -v.w};
}

template<typename T, class Tag>
inline Vec4T<T, Tag>  operator+(const Vec4T<T, Tag>& a, const Vec4T<T, Tag>& b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

template<typename T, class Tag>
inline Vec4T<T, Tag>& operator += (Vec4T<T, Tag>& a, const Vec4T<T, Tag>& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;

	return a;
}

template<typename T, class Tag>
inline Vec4T<T, Tag>  operator-(const Vec4T<T, Tag>& a, const Vec4T<T, Tag>& b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

template<typename T, class Tag>
inline Vec4T<T, Tag>& operator -= (const Vec4T<T, Tag>& a, const Vec4T<T, Tag>& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;

	return a;
}

template<typename T, class Tag>
inline Vec4T<T, Tag>& operator *= (Vec4T<T, Tag>& a, T v)
{
	a.x *= v;
	a.y *= v;
	a.z *= v;
	a.w *= v;

	return a;
}

/*
 template<typename T, class Tag>
 vec4 vec4::operator * (Matrix const& m)
{
 return vec4(Dot(m[0]), Dot(m[1]), Dot(m[2]), Dot(m[3]));

}*/

template<typename T, class Tag>
inline Vec4T<T, Tag>  operator / (const Vec4T<T, Tag>& a, T v) {
	return { a.x/v, a.y/v, a.z/v, a.w/v };
}

template<typename T, class Tag>
inline Vec4T<T, Tag>& operator /= (Vec4T<T, Tag>& a, T v)
{
	a.x /= v;
	a.y /= v;
	a.z /= v;
	a.w /= v;

	return a;
}

template<typename T, class Tag>
inline bool operator == (const Vec4T<T, Tag>& a, const Vec4T<T, Tag>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template<typename T, class Tag>
inline bool operator != (const Vec4T<T, Tag>& a, const Vec4T<T, Tag>& b)
{
	return !(a == b);
}

/*
inline ostream& operator << (ostream& os, const Vec4T& v)
{
 os << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3];

 return os;
}
 */

template<typename T, class Tag>
inline Vec4T<T, Tag> operator * (const Vec4T<T, Tag>& v, T s) {
	return {v.x*s, v.y*s, v.z*s, v.w*s};
}

template<typename T, class Tag>
inline Vec4T<T, Tag> operator * (T s, const Vec4T<T, Tag>& v) {
	return {v.x*s, v.y*s, v.z*s, v.w*s};
}

// ------------------------------------------------

template<typename T, class Tag>
inline Vec4T<T,Tag> mul(Vec4T<T,Tag> a, const Vec4T<T,Tag>& b)
{
	return Vec4T<T,Tag>(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

// ------------------------------------------------

//	using Vec4List = std::vector<Vec4>;

static_assert(std::is_pod<Vec4>::value,                "is_pod");
static_assert(std::is_standard_layout<Vec4>::value,    "is_standard_layout");
static_assert(std::is_trivial<Vec4>::value,            "is_trivial");
static_assert(std::is_trivially_copyable<Vec4>::value, "is_trivially_copyable");

// ------------------------------------------------

} // namespace emath
