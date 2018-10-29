#pragma once

#include "fwd.hpp"

namespace emath {

struct Circle
{
	Vec2f p;
	float rad;

	bool contains(const Vec2f& pos)
	{
		return distance_sq(this->p, pos) <= sqr(rad);
	}
};

static_assert(sizeof(Circle) == sizeof(Vec2f) + sizeof(float), "Pack");
static_assert(sizeof(Circle) == 3 * sizeof(float), "Pack");

} // namespace emath
