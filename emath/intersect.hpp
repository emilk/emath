#pragma once

#include "fwd.hpp"

namespace emath
{
	class Plane;

	namespace intersect
	{
		/* true if entire box is on the positive side of the plane. */
		bool box_outside_plane(const Plane& p, const Vec3& center, const Vec3& extent);
	}
}
