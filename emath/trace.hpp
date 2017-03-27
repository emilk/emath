#pragma once

#include "fwd.hpp"
#include "aabb.hpp"
#include "ray.hpp"

namespace emath
{
	namespace trace
	{
		struct info {
			enum Flags {
				COLLIDE_ENTERING  = 1,
				COLLIDE_LEAVING   = 2
			};

			Ray ray;
			real t;        // Returned t is strictly smaller than this.

			Flags flags = COLLIDE_ENTERING;
			real min_t=0;  // Returned t is stricty larger than this. min_t MUST be non-negative!
			Vec2 normal_dir; // May not be unit-length.
			// WILL HOLD: assert(dot(ti.normal_dir, ti.ray.d) <= 0); - even for 'COLLIDE_LEAVING'

			info(const Ray& r, real max_t, Flags f = COLLIDE_ENTERING) : ray(r), t(max_t), flags(f) {}

			// helpers
			bool entering() const { return flags & COLLIDE_ENTERING; }
			bool leaving()  const { return flags & COLLIDE_LEAVING;  }
			bool matches(bool ray_is_entering) {
				if (ray_is_entering && entering()) return true;
				if (!ray_is_entering && leaving()) return true;
				return false;
			}

			void sanity_check() const
			{
				assert(entering() != leaving());
				assert(0 <= min_t);
			}
		};

		/* intersect with a t less than the given, and greater or equal to 0.
		 Only entering rays will be considered.
		 */
		bool ray_circle(info& ti, const Circle& c);
		bool ray_capsule(info& ti, const CapsuleBaked& c);
		bool rayAABB(info& ti, const AABB& aabb);

		/* The circle morphs from c_0 to c_1 in t=[0,1].
		   Find intersection with ray, at ray.at(t) with same t.
		 */
		bool ray_circleCCD(info& ti, const Circle& c_0, const Circle& c_1);
	}
}
