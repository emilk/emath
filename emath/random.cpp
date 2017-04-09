//  Created by Emil Ernerfeldt on 2012-06-09.

#include "random.hpp"

unsigned random_seed()
{
#if DEBUG
	return 9;
#else
	return arc4random();
#endif
}

namespace emath {

unsigned Random::s_seed = random_seed();
Random Random::s_random;

Random::Random(const char* seed) : Random((unsigned)std::hash<const char*>()(seed))
{
}

Vec2f Random::random_unit_circle()
{
	for (;;) {
		Vec2f v = { random_interval(-1.0f, +1.0f), random_interval(-1.0f, +1.0f) };
		if (length_sq(v) <= 1.0f) { return v; }
	}
}

inline float rgb_intensity(const Vec3f& v)
{
	return 0.3f*v.r + 0.59f*v.g + 0.11f*v.b;
}

Vec3f Random::dark_rgb()
{
	for (;;) {
		Vec3f rgb = { random_float(), random_float(), random_float() };
		float intensity = rgb_intensity(rgb);
		if (intensity < 0.3f) { return rgb; }
	}
}

Vec3f Random::bright_rgb()
{
	for (;;) {
		Vec3f rgb = { random_float(), random_float(), random_float() };
		float intensity = rgb_intensity(rgb);
		if (intensity > 0.5f) { return rgb; }
	}
}

} // namespace emath
