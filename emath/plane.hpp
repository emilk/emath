#pragma once

#include "mat4.hpp"

namespace emath
{
	class Plane
	{
	public:
		Plane() : _normal(0,0,0), _dist(0) {}

		Plane(real a, real b, real c, real d)
		: _normal(a, b, c), _dist(d)
		{
			normalize();
		}

		Plane(const Vec3& v, real d)
		: _normal(v), _dist(d)
		{
			normalize();
		}

		explicit Plane(const Vec4& v)
		: _normal(v.x, v.y, v.z), _dist(v.w)
		{
			normalize();
		}

		// Created a plane of points x where Dot(x - point, normal) == 0
		static Plane from_point_normal(const Vec3& point, const Vec3& normal);
		static Plane from_points(const Vec3& p1, const Vec3& p2, const Vec3& p3);

		// Returns the intersection point of three planes.
		// NOT TESTED in 'Voxels'
		static Vec3 plane_intersection(const Plane& a, const Plane& b, const Plane& c);


		const Vec3& normal() const { return _normal; }
		real distance() const { return _dist; }


#if 0
		void transform(const Mat4& transform);
#endif


		// Only works for normalized planes.
		real distance(const Vec3& p) const
		{
			return dot(p, _normal) + _dist;
		}

	private:
		void normalize();

		// A point x is on the plane if dot(_normal, x) + _dist = 0.
		Vec3 _normal;
		real _dist;
	};


	// ---------------------------------------


	struct plane_2d
	{
		// A point x is on the plane if dot(_normal, x) + _dist = 0.
		Vec2 _normal;
		real _dist;

		// Only works for normalized planes.
		real distance(const Vec2& p) const
		{
			return dot(p, _normal) + _dist;
		}

		// Project onto plane so that distance to returned point == 'desired_dist'.
		Vec2 project(Vec2 p, float desired_dist = 0) const
		{
			float current_dist = distance(p);
			float delta = desired_dist - current_dist;
			p += _normal * delta;
			return p;
		}
	};
}
