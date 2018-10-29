#pragma once

#include <cassert>

#include "aabb.hpp"
#include "circle.hpp"
#include "fwd.hpp"
#include "line_seg.hpp"

namespace emath {

struct Capsule
{
	union {
		struct {
			union {
				Vec2f p[2];
				struct { Vec2f p0, p1; };
				LineSeg line_seg;
			};
			float rad; // >= 0 please
		};

		struct {
			Vec2f p0_again;
			Circle circ_1;
		};
	};

	Capsule() = default; // Quick

	Capsule(const Vec2f& p0_, const Vec2f& p1_, float rad_) {
		p[0] = p0_;
		p[1] = p1_;
		rad = rad_;
	}

	// p0==p1, rad can be anything >=0.
	bool is_circle() const { return p[0]==p[1]; }

	Circle& circle()
	{
		assert(is_circle());
		return circ_1;
	}
	const Circle& circle() const
	{
		assert(is_circle());
		return circ_1;
	}

	// Endpoints:
	Circle circle_0() const
	{
		return Circle{p0, rad};
	}

	const Circle& circle_1() const
	{
		return circ_1;
	}

	emath::Vec2f dir() const
	{
		return p1-p0;
	}

	emath::Vec2f dir_unit() const
	{
		return normalized(dir());
	}

	float radius() const { return rad; }

	// ------------------------------------------------
	// utils

	Capsule enlarged(float rad_delta) const
	{
		Capsule ret = *this;
		ret.rad += rad_delta; // It changes nothing else
		return ret;
	}

	static bool intersects(const Capsule& a, const Capsule& b);
};

// TODO
//static_assert(MEMBER_OFFSET(Capsule, circ_1) == MEMBER_OFFSET(Capsule, p[1]), "Pack");
static_assert(sizeof(Capsule) == sizeof(LineSeg) + sizeof(float), "Pack");
static_assert(sizeof(Capsule) == sizeof(Vec2f) + sizeof(Circle), "Pack");

static_assert(std::is_pod<Capsule>::value,                "is_pod");
static_assert(std::is_standard_layout<Capsule>::value,    "is_standard_layout");
static_assert(std::is_trivial<Capsule>::value,            "is_trivial");
static_assert(std::is_trivially_copyable<Capsule>::value, "is_trivially_copyable");

// ------------------------------------------------

inline float distance(const Capsule& cap, const Vec2f& p)
{
	Vec2f ls_closest = cap.line_seg.closest_point(p);
	return distance(ls_closest, p) - cap.rad;
}

inline float distance(const Capsule& cap, const Circle& circ)
{
	return distance(cap, circ.p)-circ.rad;
}

inline bool operator==(const Capsule& a, const Capsule& b)
{
	return a.p0==b.p0 && a.p1==b.p1 && a.rad==b.rad;
}

// ------------------------------------------------

// Capsule with extra data for quick tracing.
struct CapsuleBaked : public Capsule
{
public:
	Vec2f A,N; // Base vectors for coordinate transform
	float length; // Distance between cap.p0 and cap.p1.

	CapsuleBaked()=default;

	explicit CapsuleBaked(const Capsule& c) : Capsule(c)
	{
		A = p1 - p0;
		length = normalize(A);
		N = rot90CW(A);
	}

	CapsuleBaked enlarged(float rad_delta) const
	{
		CapsuleBaked ret = *this;
		ret.rad += rad_delta; // It changes nothing else
		return ret;
	}
};

// ------------------------------------------------

// line seg functions:

bool line_segment_intersect_test(const Vec2f& a0, const Vec2f& a1, const Vec2f& b0, const Vec2f& b1, float& t0, float& t1);
// Returns the point on the line closest to the point v.
// Returns the interpolation factor t between the points.
// return t=0 on fail
float closest_point_on_line(const Vec2f& p0, const Vec2f& p1, const Vec2f& v);

// Returns the point on the line segment closest to the point v.
// Also returns the interpolation factor t between the points. t is clamped to [0,1]
// Returns p0 on fail
Vec2f closest_point(const Vec2f& p0, const Vec2f& p1, const Vec2f& v, float& t);

Vec2f closest_point(Vec2f p0, Vec2f p1, const Vec2f& v);

// ------------------------------------------------
// Intersection tests

bool intersects(const Capsule& cap, const AABBf& rect);

} // namespace emath
