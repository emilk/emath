#include "plane.hpp"

#include "mat3.hpp"

namespace emath
{
	Plane Plane::from_point_normal(const Vec3f& p, const Vec3f& n)
	{
		return Plane(n, -dot(p, n));
	}

	Plane Plane::from_points(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3)
	{
		return from_point_normal(p1, cross(p1-p2, p3-p2));
	}

	// NOT TESTED in 'Voxels'
	Vec3f Plane::plane_intersection(const Plane& a, const Plane& b, const Plane& c)
	{
		/*
		 Alternate homebrew solution (by emilk):
		 Mat3 a(a.normal(), b.normal(), c.normal());
		 Vec3f b(-a.distance(),  -b.distance(),  -c.distance());
		 a * x = b;, so x = inverse(a) * b; Easy as pie =)
		 Hasn't yet been tested, but seems sound.
		 Infact, it seems to be exactly what is going on below, only more verbose.
		 */

		Plane planes[3]	= {a,b,c};
		Vec3f  normals[3];

		for (int i=0; i<3; ++i) {
			normals[i] = planes[i].normal();
		}

		float det =
			+ normals[0].x * normals[1].y * normals[2].z
			+ normals[0].y * normals[1].z * normals[2].x
			+ normals[0].z * normals[1].x * normals[2].y

			- normals[0].x * normals[1].z * normals[2].y
			- normals[0].y * normals[1].x * normals[2].z
			- normals[0].z * normals[1].y * normals[2].x;

		Vec3f top =
			cross(normals[1], normals[2]) * -planes[0].distance() +
			cross(normals[2], normals[0]) * -planes[1].distance() +
			cross(normals[0], normals[1]) * -planes[2].distance();

		return top / det;
	}

#if 0
	void Plane::transform(const Mat4& transform)
	{
		//
		// m = Top-left 9 elements of transform transposed and inverted
		// Is used to transform the normal
		Mat3 m(transform.M(0,0), transform.M(0,1), transform.M(0,2),
				 transform.M(1,0), transform.M(1,1), transform.M(1,2),
				 transform.M(2,0), transform.M(2,1), transform.M(2,2));
		m = transposed(inverted(m));

		// A point on the old plane
		Vec3f point = -_normal * _dist;

		//ASSERT_CODE(IsZero(0.01f*Distance(point)));

		// Transform the normal
		_normal =  m * _normal;

		// Transform the point on the plane
		Vec3f new_point = TransformCoordinate(transform, point);

		// And calculate the distance to the new point
		_dist = -dot(_normal, new_point);

		//ASSERT_CODE(IsZero(0.01f*Distance(new_point)));

		normalize();
	}
#endif

	void Plane::normalize()
	{
		auto inv_length = 1.0f / length(_normal);
		_normal *= inv_length;
		_dist   *= inv_length;
	}
}
