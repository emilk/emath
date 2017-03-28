//  Created by emilk on 2012-10-31.
#pragma once

#include "Vec2.hpp"

namespace emath {

struct Ray
{
	Vec2 o; // Origin
	Vec2 d; // Does not need to be unit length

	constexpr const Vec2& origin() const { return o; }
	constexpr const Vec2& dir() const { return d; }
	constexpr Vec2 at(real t) const { return o + t*d; }
};

// If possible, find t0 and t1 so that p0+t0*d0 = p1+t1*d1
bool ray_ray(Vec2 p0, Vec2 d0, Vec2 p1, Vec2 d1, real& t0, real& t1);

} // namespace emath
