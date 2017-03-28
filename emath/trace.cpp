//  Created by Emil Ernerfeldt on 2012-10-11.

#include "trace.hpp"

#include "capsule.hpp"
#include "circle.hpp"

namespace emath {
namespace trace {

/* Ray does not need to be normalized. */
bool ray_circle(info& ti, const Circle& c)
{
	ti.sanity_check();

	assert(ti.entering() != ti.leaving()); // Choose just one, or remove optimizations
	const bool entering = ti.entering();
	const real sign = (entering ? -1 : +1);

	const auto& ray = ti.ray;

	const Vec2 R = ray.o - c.p;
	const real incl = dot(ray.d, R);

	//if (incl*sign > 0)
	//	return false; // heading the wrong way
	if (entering && incl>0)
		return false; // early out: not heading towards us

	/* If the ray is heading towards our center, it can both enter and leave us.
	   but if it is heading away from us, it can only leave ut. */

	// x^2 * length_sq(dir) + x*2*dot(dir,R) + length_sq(R) - Sqr(r) = 0
	const real d2inv = 1 / length_sq(ray.d);
	const real a = incl * d2inv;
	const real b = (length_sq(R) - sqr(c.rad)) * d2inv;
	// Solve x^2 + x*2a + b = 0
	const real under_sq = sqr(a)-b;

	if (under_sq <= 0)
		return false; // ray does not intersect circle

#if 1

	if (sign * sqr(a + ti.t    ) < sign * under_sq)
		return false; // Too far away
	if (sign * sqr(a + ti.min_t) > sign * under_sq)
		return false; // Too close (i.e. behind us)

	// Find the root of interest:
	real x = -a + sign * std::sqrt(under_sq);

	assert(x <= ti.t     + 1e-5f); // Not too far away
	assert(x >= ti.min_t - 1e-5f); // Not too close

	// More exact tests (really only fail on x==ti.min_t (common case == 0), or rounding errors)
	if (x <= ti.min_t)
		return false; // Just missed it!
	if (x > ti.t)
		return false; // Not yet

	x = clamp<real>(x, ti.min_t, ti.t); // Fix rounding errors

	ti.t = x;
	ti.normal_dir = -sign * (ray.d*x + R);
	assert(dot(ti.normal_dir, ray.d) <= 0);

	return true;
#else
	// allows for both ti.entering() && ti.leaving() at the same time

	real under = sqrtf(under_sq);

	if (ti.entering())
	{
		// Check first root (lowest t)
		real x = -a - under; // first root

		if (x >= ti.t)
			return false; // First hit too far away

		if (ti.min_t <= x)
		{
			// first root good, no need to check second
			ti.t = x;
			ti.normal_dir = ray.d*x + R; // normalization postponed.
			return true;
		}
	}

	if (ti.flags & info::COLLIDE_LEAVING)
	{
		real x = -a + under; // second root

		if (x >= ti.t)
			return false; // Scond hit too far away

		if (ti.min_t <= x)
		{
			ti.t = x;
			ti.normal_dir = ray.d*x + R;
			return true;
		}
	}

	return false;
#endif
}

/* intersect with a t less or equal to the given. */
/* Ray does not need to be normalized. */
bool ray_capsule(info& ti, const CapsuleBaked& cap)
{
	ti.sanity_check();

	if (cap.is_circle())
		return ray_circle(ti, cap.circle());

	const auto& ray = ti.ray;
	const auto& A = cap.A;
	const auto& N = cap.N;

	// Transform ray to local capsule coords:
	Vec2 R = ray.o - cap.p0;
	Vec2 p = {dot(A, R    ),  dot(N, R    )};
	Vec2 d = {dot(A, ray.d),  dot(N, ray.d)};

	// x is along capsule, y is along normal.

	if (!is_zero(d.y)) {
		// TODO OPTIMIZE: calc just one of these for greater speed
		real tMin = (-cap.rad - p.y) / d.y;
		real tMax = (+cap.rad - p.y) / d.y;

		// Just test entering:
		assert(ti.entering());
		real t_test = std::min(tMin, tMax);
		real s = p.x + t_test * d.x;

		if (s < 0) {
			return ray_circle(ti, cap.circle_0());
		}
		else if (cap.length < s) {
			return ray_circle(ti, cap.circle_1());
		}
		else if (ti.min_t<=t_test && t_test<ti.t)  {
			ti.t = t_test;
			ti.normal_dir = (d.y < 0 ?  N : -N);

			assert(dot(ray.d, ti.normal_dir) < 0);
			assert(cap.line_seg.is_approached_by(ray));

			return true;
		}
		else {
			return false;
		}
	} else  {
		// Special case: Ray is parallell to capsule.
		// Test against end circles.

		bool did_hit=false;
		if (ray_circle(ti, cap.circle_0())) did_hit = true;
		if (ray_circle(ti, cap.circle_1())) did_hit = true;
		return did_hit;
	}
}

//------------------------------------------------------------------------------

bool rayAABB(info& ti, const AABB& aabb) {

	// p and d are the position and direction in the local coordinate system
	Vec2 p = ti.ray.origin() - aabb.center();
	Vec2 d = ti.ray.dir();
	Vec2 hs = aabb.size()/2; // half size

	bool did_hit = false;

	for (int a=0; a<2; ++a)
	{
		//if (IsZero(d[a])) continue; // Parallel

		// Intersect near and far-plane of axis 'a':
		real tMin = (-hs[a] - p[a]) / d[a];
		real tMax = (+hs[a] - p[a]) / d[a];

		// Test first hit:
		if (ti.min_t<tMin && tMin<ti.t)
		{
			// The hit the min-plane, is the hit within the box on the other axis?
			real other = p[1-a]+tMin*d[1-a];
			if (emath::abs(other) <= hs[1-a])  // <= vs < is to catch symmetry cases, i.e. when hitting both planes at the same time
			{
				if (ti.matches(d[a]>0))
				{
					did_hit = true;
					ti.t = tMin;
					ti.normal_dir = Vec2(0,0);
					ti.normal_dir[a] = -1;
				}
			}
		}

		// Test second hit:
		if (ti.min_t<tMax && tMax<ti.t)
		{
			// The hit the max-plane, is the hit within the box on the other axis?
			real other = p[1-a]+tMax*d[1-a];
			if (emath::abs(other) <= hs[1-a])  // <= vs < is to catch symmetry cases, i.e. when hitting both planes at the same time
			{
				if (ti.matches(d[a]<0))
				{
					did_hit = true;
					ti.t = tMax;
					ti.normal_dir = Vec2(0,0);
					ti.normal_dir[a] = +1;
				}
			}
		}
	}

	return did_hit;
}

//------------------------------------------------------------------------------

enum Root {
	Min,
	Max
};

/* a*x + b = 0
 Solve for x
 with x = [min_x, max_x]
 */
real solve_X_C(real a, real b)
{
	assert(a!=0);
	return -b/a;
}

/* x^2 + b*x + c = 0
 Solve for x
 with x = [min_x, max_x]

 If two solutions is found, rp(x0,x1) is returned.
 */
template<class RootPicker>
real solve_X2_X_C(real b, real c, const RootPicker& rp)
{
	if (c==0) {
		// Trivial case of x^2 + b*x = 0
		// one root is x=0
		// the other is x=-b
		return rp(0, -b);
	}
	else {
		real b_2 = b/2;
		real under_sqrt = sqr(b_2) - c;
		if (under_sqrt < 0)
		{
			// No real solutions
			return NAN;
		}
		else if (under_sqrt == 0)
		{
			// Special case: just one solution
			return -b_2;
		} else {
			real s = std::sqrt(under_sqrt);
			return rp(-b_2 - s,
						 -b_2 + s
						 );
		}
	}
}

/* a*x^2 + b*x + c = 0
 Solve for x
 with x = [min_x, max_x]
 */
template<class RootPicker>
real solve_X2_X_C(real a, real b, real c, const RootPicker& rp)
{
	if (a==0) {
		// Linear
		return solve_X_C(b, c);
	} else {
		return solve_X2_X_C(b/a, c/a, rp);
	}
}

bool ray_circleCCD(info& ti, const Circle& c_0, const Circle& c_1)
{
	ti.sanity_check();

	assert(ti.entering() != ti.leaving()); // One or the other.
	assert(0 <= ti.min_t);
	assert(ti.t <= 1);

	/*
	 Given a circle that moves from [c0,r0] to [c1,r1] in t=[0,1]
	 Trace against ray that goes from o0 to o1, also in t=[0,1]

	 Nomenclature:
	   Xd = X1-X0
	   A.B = dot(a,b)

	 // Definitions:
	 P = o0 + t*od   // Ray hit point
	 C = c0 + t*cd   // Hit circle center
	 R = r0 + t*rd   // Hit radius

	 // Lemma:
	 (C-P)^2 = R^2

	 // Work:
	 C-P = c0-o0 + t*(cd-od);
	 V = c0-o0
	 D = cd-od
	 C-P = V + t*D
	 (C-P)^2 = (V + t*D)^2 = V^2 + 2*t*(D.V) + t^2*D^2

	 R^2 = (r0 + t*rd)^2 = r0^2 + 2*t*r0*rd + t^2*rd^2

	 // Putting it all together:
	 V^2 + 2*t*(D.V) + t^2*D^2 = r0^2 + 2*t*r0*rd + t^2*rd^2

	 t^2 * (D^2 - rd^2)  +  t * 2*((D.V) - (r0*rd)) + V^2-r0^2 = 0;

	 // standard form:
	 a = (D^2 - rd^2);
	 b = 2*((D.V) - (r0*rd))
	 c = V^2-r0^2

	 a*t^2 + b*t + c = 0.
	 */

	// Setup input in the format described above

	Vec2  o0 = ti.ray.origin();
	Vec2  od = ti.ray.dir();
	Vec2  c0 = c_0.p;
	real r0 = c_0.rad;
	Vec2  c1 = c_1.p;
	real r1 = c_1.rad;

	// GO:

	Vec2  cd = c1-c0;
	real rd = r1-r0;

	Vec2  V = c0-o0;
	Vec2  D = cd-od;
	real a = sqr(D) - sqr(rd);
	real b = 2*(dot(D,V) - (r0*rd));
	real c = sqr(V) - sqr(r0);

	real sign = ti.entering() ? -1 : +1;

	/* a*x^2 + b*x + c = 0
	   Solve for x, and pick the smallest root if entering, else largest root:
	 */
	real x = solve_X2_X_C(a, b, c, [=](real x0, real x1){return sign*std::max(sign*x0, sign*x1);});

	if (isfinite(x) && ti.min_t < x && x < ti.t) {
		ti.t = x;
		ti.normal_dir = (c0 + cd*x - ti.ray.at(x)); // The sign here is impossible to get right

		if (dot(ti.normal_dir, ti.ray.d) > 0)
			ti.normal_dir = -ti.normal_dir;  // FIXME

		assert(dot(ti.normal_dir, ti.ray.d) <= 0);
		return true;
	} else {
		return false;
	}
}

} // namespace trace
} // namespace emath
