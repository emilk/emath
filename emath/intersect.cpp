//  Created by Emil Ernerfeldt on 2013-02-16.

#include "intersect.hpp"
#include "plane.hpp"
#include "vec2.hpp"

namespace emath {
namespace intersect {

bool box_outside_plane(const Plane& p, const Vec3f& c, const Vec3f& e)
{
	// TODO: OPTIMIZE
	if (p.distance(c - Vec3f(+e.x, +e.y, +e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(+e.x, +e.y, -e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(+e.x, -e.y, +e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(+e.x, -e.y, -e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(-e.x, +e.y, +e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(-e.x, +e.y, -e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(-e.x, -e.y, +e.z)) <= 0) { return false; }
	if (p.distance(c - Vec3f(-e.x, -e.y, -e.z)) <= 0) { return false; }
	return true;
}

bool circle_circle(const Circle& a, const Circle& b, Vec2f out_solutions[2])
{
	const Vec2f delta = b.p - a.p;
	const auto distance = length(delta);

	if (distance > a.rad + b.rad)
	{
		return false; // No intersection.
	}

	if (distance < std::abs(a.rad - b.rad))
	{
		return false; // One circle contains the other.
	}

	// 'p2' is the point where the line through the circle intersection points
	// crosses the line between the circle centers.

	// Determine the distance from circle_a to p2.
	auto a_p2_dist = (sqr(a.rad) - sqr(b.rad) + sqr(distance)) / (2 * distance);

	// Determine the coordinates of p2.
	auto p2 = a.p + delta * a_p2_dist / distance;

	// Determine the distance from p2 to either of the intersection points.
	auto h = std::sqrt(sqr(a.rad) - sqr(a_p2_dist));

	// Now determine the offsets of the intersection points from p2
	auto rx = -delta.y * (h/distance);
	auto ry = +delta.x * (h/distance);

	// Determine the absolute intersection points.
	out_solutions[0].x = p2.x + rx;
	out_solutions[1].x = p2.x - rx;
	out_solutions[0].y = p2.y + ry;
	out_solutions[1].y = p2.y - ry;

	return true;
}

} // namespace intersect
} // namespace emath
