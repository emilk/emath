//  Created by emilk on 2012-10-31.
#pragma once

#include "vec2.hpp"

namespace emath {

struct Ray
{
	Vec2f o; // Origin
	Vec2f d; // Does not need to be unit length

	constexpr const Vec2f& origin() const { return o; }
	constexpr const Vec2f& dir() const { return d; }
	constexpr Vec2f at(float t) const { return o + t*d; }
};

// If possible, find t0 and t1 so that p0+t0*d0 = p1+t1*d1
bool ray_ray(Vec2f p0, Vec2f d0, Vec2f p1, Vec2f d1, float& t0, float& t1);

} // namespace emath
