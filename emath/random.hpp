#pragma once

#include <random>

#include "math.hpp" // floor_to_int
#include "range.hpp"
#include "vec2.hpp" // Convenience
#include "vec3.hpp" // Convenience
#include "vec4.hpp" // Convenience

namespace emath {

class Random
{
public:
	static Random& global() { return s_random; }

	// ------------------------------------------------

	Random() : _rand(s_seed++) {}
	explicit Random(size_t seed) : _rand(seed) {}
	explicit Random(const char* seed); // Will hash 'seed'

	/// [0,1)
	inline float random_float()
	{
		return _rand() / (_rand.max() + 1.0f);
	}

	/// [0,max)
	inline float random_uniform(float max)
	{
		return random_float() * max;
	}

	inline Vec2f random_uniform(const Vec2f& max)
	{
		return { random_float()*max.x, random_float()*max.y };
	}

	inline Vec3f random_uniform(const Vec3f& max)
	{
		return { random_float()*max.x, random_float()*max.y, random_float()*max.z };
	}

	inline float random_angle()
	{
		return random_uniform(emath::TAUf);
	}

	inline int random_sign()
	{
		return random_bool() ? +1 : -1;
	}

	/// [0, max)
	inline int random_int(int max)
	{
		return floor_to_int(random_float() * max);
	}

	/// [min, max)
	inline float random_interval(float min, float max)
	{
		return min + random_float()*(max-min);
	}

	// [min, max)
	inline float range(const RangeF& r)
	{
		return r.min() + random_float()*(r.max() - r.min());
	}

	// Gaussian distribution with mean=0 and variance=1
	float random_normal()
	{
		return _normal(_rand);
	}

	/// Returns random point with norm() <= 1.
	Vec2f random_unit_circle();

	inline Vec2f random_normal_vec2()
	{
		return { random_normal(), random_normal() };
	}

	inline Vec3f random_normal_vec3()
	{
		return { random_normal(), random_normal(), random_normal() };
	}

	inline Vec2f random_dir_2d()
	{
		return vec2_angled(random_angle());
	}

	inline Vec3f random_dir_3d()
	{
		Vec3f ret;
		do ret = random_normal_vec3(); while (length_sq(ret) == 0);
		return normalized(ret);
	}

	inline bool random_bool()
	{
		return (_rand() & 1)!=0;
	}

	template<class List>
	auto random_element(const List& list)
	{
		assert(!list.empty());
		return list[random_int((int)list.size())];
	}

	// Colors:
	Vec3f dark_rgb();
	Vec4f dark_rgba()   { return {dark_rgb(),   1.0f}; }
	Vec3f bright_rgb();
	Vec4f bright_rgba() { return {bright_rgb(), 1.0f}; }

private:
	using MyRNG  = std::mt19937;
	using Normal = std::normal_distribution<float>;

	MyRNG  _rand;
	Normal _normal;

	static unsigned s_seed;
	static Random s_random;
};

} // namespace emath
