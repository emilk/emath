//  Created by Emil Ernerfeldt on 2013-02-23.

#include "direction.hpp"
#include "quaternion.hpp"

namespace emath
{
	Vec3f rotate_dir_around_axis(Vec3f dir, Vec3f axis, float angle)
	{
		return transform(Quat::from_axis(axis, angle), dir);
	}

	void Direction::set_up(Vec3f up)
	{
		Vec3f d = dir();

		_up = normalized(up);

		// Make sure we look the same direction in the new coordinate system:
		set_dir(d);
	}

	Vec3f Direction::dir() const
	{
		return _dir;
	}

	void Direction::set_dir(Vec3f d)
	{
		_dir = d;
		normalize(_dir);
		set_pitch( pitch() ); // Will constrain pitch
	}

	Vec3f Direction::right() const
	{
		return normalized( cross(dir(), up()) );
	}

	float Direction::pitch() const
	{
		return std::asin(dot(dir(), up()));
	}

	void Direction::set_pitch(float p)
	{
		// Constrain input:
		const auto MaxPitch = 0.999f * 0.25f * TAUf;
		p = clamp(p, -MaxPitch, +MaxPitch);

		_dir = normalized(project_onto(dir(), up()));      // Remove pitch
		_dir = rotate_dir_around_axis(dir(), right(), p);  // Tilt up/down
		normalize(_dir);                                   // Prevent drift
	}

	void Direction::look(Vec2f delta)
	{
		_dir = rotate_dir_around_axis(dir(), up(), -delta.x);
		normalize(_dir); // Prevent drift

		auto p = pitch();
		p += delta.y;
		set_pitch(p);
	}
}
