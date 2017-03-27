//  Created by Emil Ernerfeldt on 2013-02-16.
//  Copyright (c) 2015 Emil Ernerfeldt. All rights reserved.
//

#include "intersect.hpp"
#include "plane.hpp"

namespace emath
{
	namespace intersect
	{
		/* true if entire box is on the positive side of the plane. */
		bool box_outside_plane(const Plane& p, const Vec3& c, const Vec3& e)
		{
			// TODO: OPTIMIZE
			if (p.distance(c - Vec3(+e.x, +e.y, +e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(+e.x, +e.y, -e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(+e.x, -e.y, +e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(+e.x, -e.y, -e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(-e.x, +e.y, +e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(-e.x, +e.y, -e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(-e.x, -e.y, +e.z)) <= 0) { return false; }
			if (p.distance(c - Vec3(-e.x, -e.y, -e.z)) <= 0) { return false; }
			return true;
		}
	}
}
