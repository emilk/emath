#pragma once

#include "fwd.hpp"

namespace emath
{
	/*
	 Encapsulates a viewing direction.
	 Useful for player looking direction.
	 All these are meaningless without a well-defined up-vector.
	 */
	class Direction
	{
	public:
		/// Look along +x with up at +z
		Direction() { }

		Vec3f up() const { return _up; }
		void set_up(Vec3f up); // Will persist 'dir()'

		Vec3f dir() const;
		void set_dir(Vec3f dir);

		// projected onto the 'up' plane
		Vec3f right() const;

		float pitch() const;
		void set_pitch(float p);

		// Provided: screenspace x/y delta in radians
		void look(Vec2f delta);

	private:
		Vec3f  _up  {0, 0, 1};
		Vec3f  _dir {1, 0, 0};
	};
}
