#pragma once
/*
 Author: Emil Ernerfeldt
 Created for Life, 2007-01
 Adopted from DH-Demo 2005
 Adapted for Voxel Shooter 2013
 */

#include "vec3.hpp"
#include "mat4.hpp"

namespace emath
{
	class Plane
	{
	public:
		Plane() : _normal(0,0,0), _dist(0) {}

		Plane(float a, float b, float c, float d) : _normal(a, b, c), _dist(d)
		{
			normalize();
		}

		Plane(const Vec3f& v, float d) : _normal(v), _dist(d)
		{
			normalize();
		}

		explicit Plane(const Vec4f& v) : _normal(v.x, v.y, v.z), _dist(v.w)
		{
			normalize();
		}

		// Created a plane of points x where Dot(x - point, normal) == 0
		static Plane from_point_normal(const Vec3f& point, const Vec3f& normal);
		static Plane from_points(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3);

		// Returns the intersection point of three planes.
		// NOT TESTED in 'Voxels'
		static Vec3f plane_intersection(const Plane& a, const Plane& b, const Plane& c);

		const Vec3f& normal() const { return _normal; }
		float distance() const { return _dist; }

#if 0
		void transform(const Mat4& transform);
#endif

		// Only works for normalized planes.
		float distance(const Vec3f& p) const
		{
			return dot(p, _normal) + _dist;
		}

	private:
		void normalize();

		// A point x is on the plane if dot(_normal, x) + _dist = 0.
		Vec3f _normal;
		float _dist;
	};

	// ---------------------------------------

	struct plane_2d
	{
		// A point x is on the plane if dot(_normal, x) + _dist = 0.
		Vec2f _normal;
		float _dist;

		// Only works for normalized planes.
		float distance(const Vec2f& p) const
		{
			return dot(p, _normal) + _dist;
		}

		// Project onto plane so that distance to returned point == 'desired_dist'.
		Vec2f project(Vec2f p, float desired_dist = 0) const
		{
			float current_dist = distance(p);
			float delta = desired_dist - current_dist;
			p += _normal * delta;
			return p;
		}
	};
}
