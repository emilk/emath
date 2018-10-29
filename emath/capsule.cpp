//  Created by Emil Ernerfeldt on 2012-09-10.

#include "capsule.hpp"
#include "aabb.hpp"

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

// ------------------------------------------------

// Is 'point' left of the line from p0 to p1?
inline bool is_point_left_of(Vec2f p0, Vec2f p1, Vec2f point)
{
	// There's room for optimization here
	return dot(rot90CCW(p1-p0), point-p0) > 0;
}

// If possible, find t0 and t1 so that p0+t0*d0 = p1+t1*d1
bool ray_ray(Vec2f p0, Vec2f d0, Vec2f p1, Vec2f d1, float& t0, float& t1)
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

	//if (emath::IsZero(det)) return false;

	t0 = (float)((E*D - B*F) / det);
	t1 = (float)((A*F - E*C) / det);

	return std::isfinite(t0) && std::isfinite(t1);
}

#if 0
// Find
inline bool capsule_capsule(float& t0, float& t1, Vec2f p0, Vec2f d0, Vec2f p1, Vec2f d1, float r0, float r1)
{
	if (! ray_ray(p0,d0,p1,d1,t0,t1))
		return false;

	/* Ensure distance(p0+t0*d0, p1+t1*d1) = r0+r1;
	 */
}
#endif

inline bool line_segment_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& b0, const Vec2f& b1)
{
#if 1
	// WARNING! VEC ON VEC WILL FAIL IN THIS PATH. CSG::Cuts depends on this! SO USE THIS PATH
	//Optimization time: the == are true if both sides have the same sign
	//This is the same as multiplying them and making sure the result is positive

	if (is_point_left_of(a0, a1, b0) == is_point_left_of(a0, a1, b1)) {
		return false;
	}

	if (is_point_left_of(b0, b1, a0) == is_point_left_of(b0, b1, a1)) {
		return false;
	}
#else
	// so, >= or >? >= will include hits if any vertices are the same. seems reasonable to me that.

	 const Vec2f& a = emath::Rot90CCW(a1-a0);
	 if (Dot(a, b0-a0) * Dot(a, b1-a0) > 0) // >= 0
	 return false; // Both the same sign, or at least one is zero

	 const Vec2f& b = emath::Rot90CCW(b1-b0);
	 if (Dot(b, a0-b0) * Dot(b, a1-b0) > 0) // >= 0
	 return false; // Both the same sign, or at least one is zero
#endif

	// This means there MUST be an intersection!

	return true;
}

bool line_segment_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& b0, const Vec2f& b1, float& t0, float& t1)
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
	if (line_segment_intersect_test(a0, a1, b0, b1, t0, t1)) {
		if (point) {
			*point = a0 + t0*(a1-a0);
		}
		return true;
	}
	return false;
}

inline bool line_segment_ray_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& pos, const Vec2f& dir, float minT, float& t)
{
	float t0, t1;
	if (ray_ray(a0, a1-a0, pos, dir, t0, t1)) {
		if (0<=t0 && t0<1 && minT<=t1 && t1<t) {
			t = t1;
			return true;
		}
	}
	return false;
}

// ------------------------------------------------

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
// LineSeg

Vec2f LineSeg::closest_point(const Vec2f& pos) const
{
	return emath::closest_point(p[0], p[1], pos);
}

float LineSeg::closestT(const Vec2f& pos) const
{
	float t;
	emath::closest_point(p0, p1, pos, t);
	return t;
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

bool intersects(const Capsule& cap, const AABBf& rect)
{
	// TODO: more exact please
	return cap.line_seg.distance_sq_to(rect.center()) < sqr(cap.radius() + 0.5f * length( rect.size() ));
}

} // namespace emath
