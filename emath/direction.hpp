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

		Vec3 up() const { return _up; }
		void set_up(Vec3 up); // Will persist 'dir()'

		Vec3 dir() const;
		void set_dir(Vec3 dir);

		// projected onto the 'up' plane
		Vec3 right() const;

		real pitch() const;
		void set_pitch(real p);

		// Provided: screenspace x/y delta in radians
		void look(Vec2 delta);

	private:
		Vec3  _up  {0, 0, 1};
		Vec3  _dir {1, 0, 0};
	};
}
