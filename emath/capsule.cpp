//  Created by Emil Ernerfeldt on 2012-09-10.
#pragma once

#include "aabb.hpp"
#include "capsule.hpp"
#include "line_seg.hpp"

namespace emath {

// ------------------------------------------------
// Code from http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm

#define SMALL_NUM  0.00000001 // anything that avoids division overflow

/* Works in 2D and 3D */
template<class Segment>
void closest_lineseg_lineseg(
	float& out_t0, float& out_t1, float& out_dist_sq, Segment s0, Segment s1)
{
	// Three vectors:
	auto d0 = s0.p1 - s0.p0;  // s0 dir
	auto d1 = s1.p1 - s1.p0;  // s1 dir
	auto w  = s0.p0 - s1.p0;  // translation between origins
	float    a = dot(d0,d0);        // always >= 0
	float    b = dot(d0,d1);
	float    c = dot(d1,d1);        // always >= 0
	float    d = dot(d0,w);
	float    e = dot(d1,w);
	float    D = a*c - b*b;       // always >= 0
	float    N_0, D_0 = D;      // sc = N_0 / D_0, default D_0 = D >= 0
	float    N_1, D_1 = D;      // tc = N_1 / D_1, default D_1 = D >= 0

	// compute the line parameters of the two closest points
	if (D < SMALL_NUM) { // the lines are almost parallel
		N_0 = 0.0;        // force using point p0 on segment s0
		D_0 = 1.0;        // to prevent possible division by 0.0 later
		N_1 = e;
		D_1 = c;
	} else {   // get the closest points on the infinite lines
		N_0 = (b*e - c*d);
		N_1 = (a*e - b*d);
		if (N_0 < 0.0) {       // t0 < 0 => the s=0 edge is visible
			N_0 = 0.0;
			N_1 = e;
			D_1 = c;
		} else if (N_0 > D_0) {  // t0 > 1 => the s=1 edge is visible
			N_0 = D_0;
			N_1 = e + b;
			D_1 = c;
		}
	}

	if (N_1 < 0.0) {           // t1 < 0 => the t=0 edge is visible
		N_1 = 0.0;
		// recompute t0 for this edge
		if (-d < 0.0) {
			N_0 = 0.0;
		} else if (-d > a) {
			N_0 = D_0;
		} else {
			N_0 = -d;
			D_0 = a;
		}
	}
	else if (N_1 > D_1) {      // tc > 1 => the t=1 edge is visible
		N_1 = D_1;
		// recompute t0 for this edge
		if ((-d + b) < 0.0) {
			N_0 = 0;
		} else if ((-d + b) > a) {
			N_0 = D_0;
		} else {
			N_0 = (-d + b);
			D_0 = a;
		}
	}

	// finally do the division to get t0 and t1
	out_t0 = (abs(N_0) < SMALL_NUM ? 0.0 : N_0 / D_0);
	out_t1 = (abs(N_1) < SMALL_NUM ? 0.0 : N_1 / D_1);

	out_dist_sq = length_sq((w + out_t0*d0 - out_t1*d1));
}

// ----------------------------------------------------------------------------

// Returns the point on the line closest to the point v.
// Returns the interpolation factor t between the points.
// return t=0 on fail
float closest_point_on_line(const Vec2f& p0, const Vec2f& p1, const Vec2f& v)
{
	// p = Lerp(p0, p1, t) = p0 + t * (p1-p0)
	// Dot(v-p, p1-p0) = 0 // The closest point is orthogonal to the line
	// Dot(v - p0 - t * (p1-p0), p1-p0) = 0
	// t = Dot(v-p0, p1-p0) / (p1-p0).LengthSq()

	Vec2f axis = p1 - p0;
	auto t = dot(v-p0, axis) / length_sq(axis);

	if (!std::isfinite(t)) {
		return 0; // Badness, probably zero axis, so return an endpoint
	}

	return t;
}

// Returns the point on the line segment closest to the point v.
// Also returns the interpolation factor t between the points. t is clamped to [0,1]
// Returns p0 on fail
Vec2f closest_point(const Vec2f& p0, const Vec2f& p1, const Vec2f& v, float& t)
{
	t = closest_point_on_line(p0,p1,v);

	if (t<=0) {
		t = 0;
		return p0;
	} else if (t>=1) {
		t = 1;
		return p1;
	} else {
		return lerp(p0, p1, t);
	}
}

Vec2f closest_point(Vec2f p0, Vec2f p1, const Vec2f& v)
{
	/*
	 Vec2f axis = p1 - p0;
	 float t = Dot(v-p0, axis) / axis.LengthSq();
	 return p0 + t * axis;
	 */
	float t;
	return closest_point(p0, p1, v, t);
}

// ------------------------------------------------
// Capsule

bool Capsule::intersects(const Capsule& a, const Capsule& b)
{
	if (a.is_circle()) {
		return distance(b, a.p[0]) < a.rad;
	}

	if (b.is_circle()) {
		return distance(a, b.p[0]) < b.rad;
	}

	if (line_segment_intersect_test(a.p[0], a.p[1], b.p[0], b.p[1], nullptr)) {
		return true;
	}

	for (int i=0; i<2; ++i) {
		if (distance(a, b.p[i]) < b.rad) {
			return true;
		}
		if (distance(b, a.p[i]) < a.rad) {
			return true;
		}
	}

	return false;
}

// ------------------------------------------------

bool intersects(const Capsule& cap, const AABB2f& rect)
{
	// TODO: more exact please
	return cap.line_seg.distance_sq(rect.center()) < sqr(cap.radius() + 0.5f * length( rect.size() ));
}

} // namespace emath
