#pragma once

#include <cstdint>
#include <cstddef>

#include "real.hpp"

namespace emath {

class Random;
class Capsule;
class CapsuleBaked;
class Circle;
class Ray;

namespace trace
{
	struct info;
}

using byte = unsigned char;

// --------------------------------------------------

struct scalar_tag{};

// --------------------------------------------------

// Tag class
class zero_tag
{
public:
	static const zero_tag s_instance;

	zero_tag(const zero_tag&){}
private:
	zero_tag(){}
	zero_tag& operator=(zero_tag&){return *this;}
};

/*
 Typeless, unitless, dimensionless zero.
 Usage:: Vec2 v = Zero; assert(v == Zero);
 */
extern const zero_tag Zero;

// ------------------------------------------------

template<typename T>
class Vec2T
{
public:
	using element_type   = T;
	using iterator       = T*;
	using const_iterator = const T*;

#if 1
	T x, y;
#else
	union {
		T _v[2];
		struct { T x, y; };
		struct { T width, height; };
		//std::complex<T> cplx; // not POD
	};
#endif

	// --------------------------------

	Vec2T() = default; // Fast - no initialization!

	Vec2T(zero_tag) : x(0), y(0) { }

	Vec2T(T x_, T y_) {
		x = x_;
		y = y_;
	}

	explicit Vec2T(T a) {
		x = a;
		y = a;
	}

	template<typename F>
	explicit Vec2T(const Vec2T<F>& v) {
		x = (T)v.x;
		y = (T)v.y;
	}

	// ------------------------------------------------

	T* data()
	{
		static_assert(offsetof(Vec2T<T>, x) == 0 * sizeof(T), "Pack better");
		static_assert(offsetof(Vec2T<T>, y) == 1 * sizeof(T), "Pack better");
		return &x;
	}
	const T* data() const
	{
		static_assert(offsetof(Vec2T<T>, x) == 0 * sizeof(T), "Pack better");
		static_assert(offsetof(Vec2T<T>, y) == 1 * sizeof(T), "Pack better");
		return &x;
	}

	T  operator[](int i) const;
	T& operator[](int i);
	T  operator[](unsigned i) const;
	T& operator[](unsigned i);
};

using Vec2    = Vec2T<real>;
using Vec2f   = Vec2T<float>;
using Vec2d   = Vec2T<double>;
using Vec2i   = Vec2T<int>;
using Vec2u   = Vec2T<unsigned>;
using Vec2u16 = Vec2T<uint16_t>;
using Vec2u8  = Vec2T<uint8_t>;
using Vec2s   = Vec2T<size_t>;

// --------------------------------------------------

/* Tagging is a way to get typesafe (strong) typedefs:

 struct RGBAf_tag{};
 using RGBAf = Vec4T<float, RGBA_tag>;
 */
template<typename T, class Tag = scalar_tag>
class Vec3T
{
public:
	using element_type   = T;
	using iterator       = T*;
	using const_iterator = const T*;

#if 0
	T x, y, z;
#else
	union {
		T _v[3];
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { Vec2T<T> xy; T z_; };
	};
#endif

	// ------------------------------------------------

	Vec3T() = default; // Fast - no initialization!
	Vec3T(zero_tag) : x(0), y(0), z(0) { }
	explicit Vec3T(T v) : x(v), y(v), z(v) { }
	Vec3T(const Vec2& v, T z) : x(v.x), y(v.y), z(z) { }
	Vec3T(T x, T y, T z) : x(x), y(y), z(z) { }

	template<typename OtherTag>
	explicit Vec3T(Vec3T<T, OtherTag> v) : x(v.x), y(v.y), z(v.z) { }

	// Explicit cast:
	template<typename F, typename OtherTag>
	explicit Vec3T(const Vec3T<F, OtherTag>& v) : x((T)v.x), y((T)v.y), z((T)v.z) { }

	// ------------------------------------------------

	T*        data()        { return _v; }
	const T*  data() const  { return _v; }

	static constexpr unsigned  size()   { return 3; }
	iterator                   begin()        { return data(); }
	const_iterator             begin() const  { return data(); }
	iterator                   end()          { return data() + size(); }
	const_iterator             end() const    { return data() + size(); }

	// ------------------------------------------------

	T  operator[](int i) const { return _v[i]; }
	T& operator[](int i)       { return _v[i]; }

	T  operator[](unsigned i) const { return _v[i]; }
	T& operator[](unsigned i)       { return _v[i]; }
};

using Vec3    = Vec3T<real>;
using Vec3f   = Vec3T<float>;
using Vec3d   = Vec3T<double>;
using Vec3i   = Vec3T<int>;
using Vec3u   = Vec3T<unsigned>;
using Vec3u16 = Vec3T<uint16_t>;
using Vec3u8  = Vec3T<uint8_t>; // e.g. RGB
using Vec3s8  = Vec3T<int8_t>;  // e.g. normal

//	using Vec3List = std::vector<Vec3>;

static_assert(sizeof(Vec3u8)   == 3*sizeof(byte),      "Pack");
static_assert(sizeof(Vec3u16)  == 3*sizeof(uint16_t),  "Pack");
static_assert(sizeof(Vec3)     == 3*sizeof(real),      "Pack");

// --------------------------------------------------

/* Tagging is a hack to get typesafe (strong) typedefs:

 struct RGBAf_tag{};
 using RGBAf = Vec4T<float, RGBA_tag>;
 */
template<typename T, class Tag = scalar_tag>
class Vec4T
{
public:
	using element_type = T;
	using tag_type     = Tag;

	union {
		T _v[4];
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		struct { Vec2T<T> xy; T z_after_xy; T w_after_xy; };
		struct { Vec3T<T, Tag> rgb; T a_after_rgb; };
		struct { Vec3T<T, Tag> xyz; T w_after_xyz; };
	};

	// ------------------------------------------------

	Vec4T() = default; // Fast - no initialization!
	Vec4T(zero_tag) : x(0), y(0), z(0), w(0) { }
	explicit Vec4T(T v);
	//Vec4T(const Vec4T& v);
	//Vec4T(const Vec4T& v) = default;

	constexpr Vec4T(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) { }
	constexpr Vec4T(const Vec3T<T, Tag>& rgb_, T a_) : rgb(rgb_), a_after_rgb(a_) { }
	constexpr Vec4T(const Vec2T<T>& xy_, T z_, T w_) : xy(xy_), z_after_xy(z_), w_after_xy(w_) { }

	template<typename OtherTag>
	explicit Vec4T(Vec4T<T, OtherTag> v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

	// Explicit cast:
	template<typename F, typename OtherTag>
	explicit Vec4T(const Vec4T<F, OtherTag>& v) : x((T)v.x), y((T)v.y), z((T)v.z), w((T)v.w) { }

	// ------------------------------------------------

	T  operator[](int i) const { return _v[i]; }
	T& operator[](int i)       { return _v[i]; }

	T  operator[](unsigned i) const { return _v[i]; }
	T& operator[](unsigned i)       { return _v[i]; }

	T* data() { return _v; }
	const T* data() const { return _v; }
};

using Vec4   = Vec4T<real>;
using Vec4f  = Vec4T<float>;
using Vec4d  = Vec4T<double>;
using Vec4i  = Vec4T<int>;
using Vec4u8 = Vec4T<byte>;

static_assert(sizeof(Vec4)   == 4*sizeof(real), "Pack");
static_assert(sizeof(Vec4u8) == 4*sizeof(byte), "Pack");

// ----------------------------------------------------------------------------

template<typename T>
class Mat3T;
using Mat3f = Mat3T<float>;
using Mat3d = Mat3T<double>;

template<typename T>
class Mat4T;
using Mat4f = Mat4T<float>;
using Mat4d = Mat4T<double>;

template<typename T>
class QuaternionT;
using Quatf = QuaternionT<float>;
using Quatd = QuaternionT<double>;

template<typename T>
class AABB_T;
using AABBf = AABB_T<float>;
using AABBd = AABB_T<double>;

} // namespace emath
