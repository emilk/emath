#pragma once

#include "fwd.hpp"

namespace emath {
struct Circle;
class Plane;
namespace intersect {

/// true if entire box is on the positive side of the plane.
bool box_outside_plane(const Plane& p, const Vec3f& center, const Vec3f& extent);

inline bool cube_sphere(Vec3f cube_min, Vec3f cube_max, Vec3f sphere_center, float radius)
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

/// Return true if there was an intersection.
bool circle_circle(const Circle& a, const Circle& b, Vec2f out_solutions[2]);

} // namespace intersect
} // namespace emath
