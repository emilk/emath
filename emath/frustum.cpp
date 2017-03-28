#include "frustum.hpp"

// #if !EMILIB_GLES
// #  include <emilib/gl_lib_opengl.hpp>
// #endif

#include "intersect.hpp"
#include "vec4.hpp"

namespace emath {

Frustum Frustum::from_matrix(const Mat4& mvp)
{
	return Frustum(mvp);
}

bool Frustum::contains_point(const Vec3& p) const
{
	for (size_t i=0 ; i<NSides ; ++i) {
		if (_planes[i].distance(p) > 0) {
			return false;
		}
	}
	return true;
}

Frustum::PlaneIntersectResult Frustum::plane_intersection(const Plane& p) const
{
	int sgn = emath::sign(p.distance(_points[0]));

	for (size_t i=1; i<NPoints; ++i) {
		if (emath::sign(p.distance(_points[i])) != sgn) {
			return PlaneIntersectResult::Intersect; // We have changed sides.
		}
	}

	return sgn > 0 ? PlaneIntersectResult::Infront : sgn == 0 ? PlaneIntersectResult::Intersect : PlaneIntersectResult::Behind;
}

IntersectResult Frustum::test_sphere(const Vec3& c, float r) const
{
	bool intersects = false;

	for (size_t i=0; i<NSides; ++i) {
		real distance = _planes[i].distance(c);

		if (distance > r) {
			return IntersectResult::Outside;
		}

		if (std::fabs(distance) < r) {
			intersects = true;
		}
	}

	if (intersects) {
		return IntersectResult::Intersects;
	} else {
		return IntersectResult::Inside;
	}
}

bool Frustum::cull_box(const Vec3& c, const Vec3& e) const
{
	const auto mins = c - e;
	const auto maxs = c + e;

	#if 1
	// Test all planes against closest corner

	for (auto& p : _planes) {
		Vec3 closest = {
			p.normal().x > 0 ? mins.x : maxs.x,
			p.normal().y > 0 ? mins.y : maxs.y,
			p.normal().z > 0 ? mins.z : maxs.z
		};
		if (p.distance(closest) >= 0) {
			return true;
		}
	}

	// test all box planes against closest frustum corner:
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].x > maxs.x; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].x < mins.x; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].y > maxs.y; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].y < mins.y; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].z > maxs.z; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].z < mins.z; }
		if (out == 8) { return true; }
	}


	return false;

	#else

	// http://www.iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm

	// check box outside/inside of frustum
	for (auto& plane : _planes) {
		int out = 0;
		out += plane.distance(Vec3(mins.x, mins.y, mins.z)) > 0.0f;
		out += plane.distance(Vec3(maxs.x, mins.y, mins.z)) > 0.0f;
		out += plane.distance(Vec3(mins.x, maxs.y, mins.z)) > 0.0f;
		out += plane.distance(Vec3(maxs.x, maxs.y, mins.z)) > 0.0f;
		out += plane.distance(Vec3(mins.x, mins.y, maxs.z)) > 0.0f;
		out += plane.distance(Vec3(maxs.x, mins.y, maxs.z)) > 0.0f;
		out += plane.distance(Vec3(mins.x, maxs.y, maxs.z)) > 0.0f;
		out += plane.distance(Vec3(maxs.x, maxs.y, maxs.z)) > 0.0f;
		if (out==8) { return true; }
	}

	// check frustum outside/inside box
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].x > maxs.x; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].x < mins.x; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].y > maxs.y; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].y < mins.y; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].z > maxs.z; }
		if (out == 8) { return true; }
	}
	{
		int out = 0;
		for (int i = 0; i < 8; i++) { out += _points[i].z < mins.z; }
		if (out == 8) { return true; }
	}
	return false;
	#endif
}

Frustum::Frustum(const Mat4& mat)
{
	//Vec4 rows[4] = { mat.row(0), mat.row(1), mat.row(2), mat.row(3) };
	Vec4 rows[4] = { mat.col(0), mat.col(1), mat.col(2), mat.col(3) };

	_planes[Right]  = Plane(+rows[0] - rows[3]);
	_planes[Left]   = Plane(-rows[0] - rows[3]);
	_planes[Top]    = Plane(+rows[1] - rows[3]);
	_planes[Bottom] = Plane(-rows[1] - rows[3]);
	_planes[Back]   = Plane(+rows[2] - rows[3]);
	_planes[Front]  = Plane(-rows[2] - rows[3]);

#if 1
	// NOT TESTED in 'Voxels'
	_points[0] = Plane::plane_intersection(_planes[Back],  _planes[Left],  _planes[Bottom]);
	_points[1] = Plane::plane_intersection(_planes[Back],  _planes[Left],  _planes[Top]   );
	_points[2] = Plane::plane_intersection(_planes[Back],  _planes[Right], _planes[Bottom]);
	_points[3] = Plane::plane_intersection(_planes[Back],  _planes[Right], _planes[Top]   );

	_points[4] = Plane::plane_intersection(_planes[Front], _planes[Left],  _planes[Bottom]);
	_points[5] = Plane::plane_intersection(_planes[Front], _planes[Left],  _planes[Top]   );
	_points[6] = Plane::plane_intersection(_planes[Front], _planes[Right], _planes[Bottom]);
	_points[7] = Plane::plane_intersection(_planes[Front], _planes[Right], _planes[Top]   );
#else
	_points[0] = Vec3(-1, -1, -1);
	_points[1] = Vec3(-1, +1, -1);
	_points[2] = Vec3(+1, -1, -1);
	_points[3] = Vec3(+1, +1, -1);

	_points[4] = Vec3(-1, -1, +1);
	_points[5] = Vec3(-1, +1, +1);
	_points[6] = Vec3(+1, -1, +1);
	_points[7] = Vec3(+1, +1, +1);

	Mat4 inv = inverted(mat);

	for (uint i=0; i<NPoints; ++i) {
		_points[i] = mul_pos(inv, _points[i]);
	}
#endif
}

bool intersects(const Frustum& lhs, const Frustum& rhs)
{
	for (size_t i=0; i<Frustum::NSides; ++i) {
		if (rhs.plane_intersection(lhs._planes[i]) == Frustum::PlaneIntersectResult::Infront) {
			return false;
		}
	}

	for (size_t i=0; i<Frustum::NSides; ++i) {
		if (lhs.plane_intersection(rhs._planes[i]) == Frustum::PlaneIntersectResult::Infront) {
			return false;
		}
	}

	return true;
}

//#if !EMILIB_GLES
#if 0
void Frustum::draw() const
{
	glBegin(GL_QUADS);

	// Near.
	glVertex3fv(_points[2].data());
	glVertex3fv(_points[3].data());
	glVertex3fv(_points[1].data());
	glVertex3fv(_points[0].data());

	// Far.
	glVertex3fv(_points[4].data());
	glVertex3fv(_points[5].data());
	glVertex3fv(_points[7].data());
	glVertex3fv(_points[6].data());

	// Left
	glVertex3fv(_points[0].data());
	glVertex3fv(_points[1].data());
	glVertex3fv(_points[5].data());
	glVertex3fv(_points[4].data());

	// Right.
	glVertex3fv(_points[6].data());
	glVertex3fv(_points[7].data());
	glVertex3fv(_points[3].data());
	glVertex3fv(_points[2].data());

	// Bottom.
	glVertex3fv(_points[4].data());
	glVertex3fv(_points[6].data());
	glVertex3fv(_points[2].data());
	glVertex3fv(_points[0].data());

	// Top.
	glVertex3fv(_points[1].data());
	glVertex3fv(_points[3].data());
	glVertex3fv(_points[7].data());
	glVertex3fv(_points[5].data());

	glEnd();
}

BoundingSphere Frustum::CalcBoudingSphere() const
{
	return BoundingSphere::Build(NPoints, &_points[0]);
}
#endif

} // namespace emath
