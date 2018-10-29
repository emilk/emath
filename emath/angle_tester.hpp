#pragma once

#include <cmath>
#include <type_traits>

#include "math.hpp"

namespace emath {

/*
 The purpose of this class is to quickly compare the angle between several
 pairs of 2D or 3D vectors against some predefined cutoff angle.
 IE: "does these two vectors span an angle greater than X radians"?
 */
template<typename VecType>
class AngleTester
{
public:
	using MyFloat = typename VecType::element_type;
	static_assert(std::is_floating_point<MyFloat>::value, "Angle tester only works with floats");

	AngleTester() = default;

	/// The cutoff angle in radians
	AngleTester(MyFloat angle) : _angle(angle)
	{
		_cosSq = sqr(std::cos(angle));
	}

	MyFloat angle() const { return _angle; }

	/// Returns true if the absolute angle between v0 and v1 exceeds test angle.
	bool is_exceeded_by(const VecType& v0, const VecType& v1) const
	{
		MyFloat cos = dot(v0,v1);
		return sign(cos)*sqr(cos) < _cosSq * length_sq(v0) * length_sq(v1);
	}

	/// Returns true if abs(v.angle()) exceeds test angle.
	/// Same as is_exceeded_by(v, Vec2f(1,0))
	bool is_exceeded_by(const VecType& v) const
	{
		MyFloat cos = v.x;
		return sign(cos)*sqr(cos) < _cosSq * length_sq(v);
	}

	friend bool operator==(const AngleTester& a, const AngleTester& b)
	{
		return a.angle() == b.angle();
	}

private:
	MyFloat _angle;
	MyFloat _cosSq;
};

} // namespace emath
