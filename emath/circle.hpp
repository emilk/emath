#pragma once

#include "fwd.hpp"

namespace emath {

struct Circle
{
	Vec2 p;
	real rad;

	bool contains(const Vec2& pos)
	{
		return distance_sq(this->p, pos) <= sqr(rad);
	}
};

static_assert(sizeof(Circle) == sizeof(Vec2) + sizeof(real), "Pack");
static_assert(sizeof(Circle) == 3 * sizeof(real), "Pack");

} // namespace emath
