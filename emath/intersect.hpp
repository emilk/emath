#pragma once

#include "fwd.hpp"

namespace emath
{
	class Plane;

	namespace intersect
	{
		/* true if entire box is on the positive side of the plane. */
		bool box_outside_plane(const Plane& p, const Vec3& center, const Vec3& extent);

		inline bool cube_sphere(Vec3 cube_min, Vec3 cube_max, Vec3 sphere_center, float radius)
		{
			float dist_squared = sqr(radius);
			if      (sphere_center.x < cube_min.x) dist_squared -= sqr(sphere_center.x - cube_min.x);
			else if (sphere_center.x > cube_max.x) dist_squared -= sqr(sphere_center.x - cube_max.x);
			if      (sphere_center.y < cube_min.y) dist_squared -= sqr(sphere_center.y - cube_min.y);
			else if (sphere_center.y > cube_max.y) dist_squared -= sqr(sphere_center.y - cube_max.y);
			if      (sphere_center.z < cube_min.z) dist_squared -= sqr(sphere_center.z - cube_min.z);
			else if (sphere_center.z > cube_max.z) dist_squared -= sqr(sphere_center.z - cube_max.z);
			return dist_squared > 0;
		}
	}
}
