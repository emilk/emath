/*
 Author: Emil Ernerfeldt
 Created for DH-Demo 2005
 Adapted for Life, 2007-01
 Adapted for VoxelShooter 2013
*/

#pragma once

#include "plane.hpp"

namespace emath {

class BoundingSphere;

enum class IntersectResult { Outside, Inside, Intersects };

/// 3D view frustum.
class Frustum
{
public:
	Frustum() = default;

	static Frustum from_matrix(const Mat4& mvp);

	bool contains_point(const Vec3& p) const;

	enum class PlaneIntersectResult { Infront, Behind, Intersect };

	/// Returns Infront if the frustum is completely in front of the plane,
	/// Behind if it's completely behind, and Intersect if they intersect.
	PlaneIntersectResult plane_intersection(const Plane& p) const;

	IntersectResult test_sphere(const Vec3& c, float r) const;
	//IntersectResult test(const BoundingSphere& b);

	/// return true if we can cull it.
	bool cull_box(const Vec3& center, const Vec3& extent) const;

	friend bool intersects(const Frustum& lhs, const Frustum& rhs);

#if 0
	void draw() const;
	BoundingSphere calc_bouding_sphere() const;
#endif

private:
	Frustum(const Mat4& mvp);

	enum Side
	{
		Right  = 0,
		Left   = 1,
		Top    = 2,
		Bottom = 3,
		Back   = 4,
		Front  = 5,
	};

	static constexpr size_t NSides = 6;
	static constexpr size_t NPoints = 8;

	/* The planes are pointing outwards. That means that things
	 in front of them (p.dist(x) > 0) are outside the frustum. */
	Plane _planes[NSides];
	Vec3  _points[NPoints];
};

} // namespace emath
