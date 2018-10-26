#pragma once

#include "vec2.hpp"
#include "math.hpp"

namespace emath {

// Is 'point' left of the line from p0 to p1?
inline bool is_point_left_of(Vec2f p0, Vec2f p1, Vec2f point)
{
	return dot(rot90CCW(p1 - p0), point - p0) > 0;
}

struct LineSeg
{
	// Returns the point on the line closest to the point v.
	// Returns the interpolation factor t between the points.
	// return t=0 on fail
	static float closest_point_on_line(const Vec2f& p0, const Vec2f& p1, const Vec2f& v)
	{
		// p = lerp(p0, p1, t) = p0 + t * (p1-p0)
		// dot(v-p, p1-p0) = 0 // The closest point is orthogonal to the line
		// dot(v - p0 - t * (p1-p0), p1-p0) = 0
		// t = dot(v-p0, p1-p0) / (p1-p0).length_sq()

		Vec2f axis = p1 - p0;
		float t = dot(v-p0, axis) / length_sq(axis);

		if (!std::isfinite(t)) {
			return 0; // Badness, probably zero axis, so return an endpoint
		}

		return t;
	}

	// Returns the point on the line segment closest to the point v.
	// Also returns the interpolation factor t between the points. t is clamped to [0,1]
	// Returns p0 on fail
	static Vec2f closest_point(const Vec2f& p0, const Vec2f& p1, const Vec2f& v, float& t)
	{
		t = closest_point_on_line(p0,p1,v);

		if (t <= 0) {
			t = 0;
			return p0;
		} else if (t >= 1) {
			t = 1;
			return p1;
		} else {
			return lerp(p0, p1, t);
		}
	}

	static Vec2f closest_point(Vec2f p0, Vec2f p1, const Vec2f& v)
	{
		/*
		Vec2f axis = p1 - p0;
		float t = dot(v-p0, axis) / axis.length_sq();
		return p0 + t * axis;
		*/
		float t;
		return closest_point(p0, p1, v, t);
	}

	// ------------------------------------------------------------------------

	Vec2f p[2];

	float length() const { return emath::distance(p[0], p[1]); }

	// Assuming CCW
	Vec2f normal() const
	{
		return normalized(emath::rot90CW(p[1]-p[0]));
	}

	Vec2f interpolate(float t) const
	{
		return lerp(p[0], p[1], t);
	}

	// Returns the point on the line closest to the point v.
	// Also returns the interpolation factor t between the points
	Vec2f closest_point(const Vec2f& v, float& t) const
	{
		return closest_point(p[0], p[1], v, t);
	}

	// Returns the point on the line closest to the point v
	Vec2f closest_point(const Vec2f& v) const
	{
		return closest_point(p[0], p[1], v);
	}

	float distance_sq(const Vec2f& v) const
	{
		return emath::distance_sq(closest_point(v), v);
	}

	/* As above, but returns infinity if it's any of the two endpoints
	that is closest */
	float confined_distance_sq(const Vec2f& v) const
	{
		Vec2f closest = closest_point(v);
		if (closest == p[0] || closest == p[1]) {
			return std::numeric_limits<float>::infinity();
		} else {
			return emath::distance_sq(closest, v);
		}
	}

	float distance(const Vec2f& v) const
	{
		return std::sqrt(distance_sq(v));
	}

	// Gives positive values if the point is to the right of the line. (outside a CCW polygon)
	float signed_distance(const Vec2f& v) const
	{
		float d = this->distance(v);
		if (is_point_on_the_left(v)) {
			d = -d; // Inside a CCW polygon
		}
		return d;
	}

	// returns true iff v is on the left of the line.
	// (where p[1]-p[0] is forwards)
	bool is_point_on_the_left(const Vec2f& v) const
	{
		return is_point_left_of(p[0], p[1], v);
	}

	// Is this direction pointing from our right to our left?
	bool is_entering(const Vec2f& dir) const
	{
		Vec2f a = p[1]-p[0];
		return a.y*dir.x - a.x*dir.y < 0;
	}
};

// If possible, find t0 and t1 so that p0+t0*d0 = p1+t1*d1
inline bool ray_ray(Vec2f p0, Vec2f d0, Vec2f p1, Vec2f d1, float& t0, float& t1)
{
	/*
	p0+t0*d0 = p1+t1*d1
	t0*d0 + t1*(-d1) = p1-p0

	t0*d0.x + t1*(-d1.x) = p1.x-p0.x;
	t0*d0.y + t1*(-d1.y) = p1.y-p0.y;

	t0*A + t1*B = E;
	t0*C + t1*D = F;

	|d0.x  -d1.x| |t0| = |p1.x-p0.x|
	|d0.y  -d1.y| |t1|   |p1.y-p0.y|

	|A B| |t0| = |E|
	|C D| |t1|   |F|

	Cramers rule gives us:

	det = A*D - B*C;
	t0  = (E*D - B*F) / det;
	t1  = (A*F - E*C) / det;
	*/

	const double A =  d0.x;
	const double B = -d1.x;
	const double C =  d0.y;
	const double D = -d1.y;
	const double E = p1.x-p0.x;
	const double F = p1.y-p0.y;

	const double det = A*D - B*C;

	t0  = (float)((E*D - B*F) / det);
	t1  = (float)((A*F - E*C) / det);

	return std::isfinite(t0) && std::isfinite(t1);
}

/// Does NOT include the endpoints, just the line between them.
inline bool line_segment_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& b0, const Vec2f& b1)
{
	//Optimization time: the == are true if both sides have the same sign
	//This is the same as multiplying them and making sure the result is positive

	if (is_point_left_of(a0, a1, b0) == is_point_left_of(a0, a1, b1)) {
		return false;
	}

	if (is_point_left_of(b0, b1, a0) == is_point_left_of(b0, b1, a1)) {
		return false;
	}

	// This means there MUST be an intersection!

	return true;
}

inline bool line_segment_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& b0, const Vec2f& b1, float& t0, float& t1)
{
	if (!line_segment_intersect_test(a0, a1, b0, b1)) {
		return false;
	}

	// This means there MUST be an intersection!

	if (!ray_ray(a0, a1-a0, b0, b1-b0, t0, t1)) {
		return false; // Parallel
	}

	t0 = saturate(t0);
	t1 = saturate(t1);

	return true;
}

inline bool line_segment_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& b0, const Vec2f& b1, Vec2f* point)
{
	float t0, t1;
	if (line_segment_intersect_test(a0,a1,b0,b1,t0,t1)) {
		if (point) {
			*point = a0 + t0*(a1-a0);
		}
		return true;
	}
	return false;
}

inline bool line_segment_ray_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& pos, const Vec2f& dir, float min_t, float& t)
{
	float t0, t1;
	if (ray_ray(a0, a1 - a0, pos, dir, t0, t1)) {
		if (0 <= t0 && t0 < 1 && min_t <= t1 && t1 < t) {
			t = t1;
			return true;
		}
	}
	return false;
}

} // emath
