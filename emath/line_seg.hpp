#pragma once

#include "Vec2.hpp"
#include "ray.hpp"

namespace emath
{
	struct LineSeg // POD!
	{
		union {
			Vec2 p[2];
			struct {
				Vec2 p0;
				Vec2 p1;
			};
		};

		// ------------------------------------------------

		real length() const { return distance(p0, p1); }

		// ------------------------------------------------
		// utils

		// t = [0,1] to be on LineSeg.
		Vec2 at(float t) const { return emath::lerp(p0, p1, t); }

		Vec2 closest_point(const Vec2& pos) const;

		// Returns t in [0,1],
		real closestT(const Vec2& pos) const;

		real distance_sq_to(const Vec2& pos) const
		{
			return distance_sq(pos, closest_point(pos));
		}

		real distance_to(const Vec2& pos) const
		{
			return distance(pos, closest_point(pos));
		}

		// returns true iff point is on the left of the line.
		// (where p[1]-p[0] is forwards)
		bool is_point_on_the_left(const Vec2& point) const
		{
			// There's room for optimization here
			return dot(emath::rot90CCW(p1-p0), point-p0) > 0;
		}

		// Is this direction pointing from our left to our right?
		bool from_left_to_right(const Vec2& dir) const
		{
			/*
			 Vec2 perfect_normal_dir = Rot90CW(p[1] - p[0]);
			 return dot(perfect_normal_dir, dir) > 0;
			 */
			Vec2 a = p1-p0;
			return a.y*dir.x - a.x*dir.y > 0;
		}

		// Is the ray approaching this linesegment?
		bool is_approached_by(const Ray& ray) const
		{
			return is_point_on_the_left(ray.o) == from_left_to_right(ray.d);
		}
	};

	static_assert(sizeof(LineSeg) == 2 * sizeof(Vec2), "Pack");
	static_assert(sizeof(LineSeg) == 4 * sizeof(real), "Pack");
}
