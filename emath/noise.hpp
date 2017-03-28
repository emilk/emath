// Created 2013-02-16.
#pragma once

#include "fwd.hpp" // vec4 etc
#include "math.hpp" // real

namespace emath
{
	/*
	 2D, 3D, 4D noise.
	 Wavelength is 1.
	 returns values in [-1,+1]
	 wraps over [0,256)
	 */
	real noise_2d(real x, real y);
	real noise_3d(real x, real y, real z);
	real noise_4d(real x, real y, real z, real w);

	// Multi-octave Simplex noise
	// For each octave, a higher frequency/lower amplitude function will be added to the original.
	// The higher the persistence [0-1], the more of each succeeding octave will be added.
	// The base frequency is 1, then comes 2, 4, 8, etc.
	float octave_noise_1d(unsigned octaves, float persistence,
								 float x);
	float octave_noise_2d(unsigned octaves, float persistence,
								 float x, float y);
	float octave_noise_3d(unsigned octaves, float persistence,
								 float x, float y, float z);
	float octave_noise_4d(unsigned octaves, float persistence,
								 float x, float y, float z, float w);

	// ----------------------------------------------------------------------
	// 1d -> 2d, 3d, 4d:

	inline float octave_noise_1d_to_1d(unsigned octaves, float persistence, float x, float seed = 0)
	{
		return octave_noise_2d(octaves, persistence, x, seed);
	}

	inline Vec2 octave_noise_1d_to_2d(unsigned octaves, float persistence, float x, float seed = 0)
	{
		return {
			octave_noise_2d(octaves, persistence, x,  7 + seed),
			octave_noise_2d(octaves, persistence, x, 13 + seed),
		};
	}

	inline Vec3 octave_noise_1d_to_3d(unsigned octaves, float persistence, float x, float seed = 0)
	{
		return {
			octave_noise_2d(octaves, persistence, x,  7 + seed),
			octave_noise_2d(octaves, persistence, x, 13 + seed),
			octave_noise_2d(octaves, persistence, x, 17 + seed),
		};
	}

	inline Vec4 octave_noise_1d_to_4d(unsigned octaves, float persistence, float x, float seed = 0)
	{
		return {
			octave_noise_2d(octaves, persistence, x,  7 + seed),
			octave_noise_2d(octaves, persistence, x, 13 + seed),
			octave_noise_2d(octaves, persistence, x, 17 + seed),
			octave_noise_2d(octaves, persistence, x, 21 + seed),
		};
	}

	// ----------------------------------------------------------------------
	// 2d -> 2d, 3d, 4d:

	inline float octave_noise_2d_to_1d(unsigned octaves, float persistence, Vec2 p, float seed = 0)
	{
		return octave_noise_2d(octaves, persistence, p.x, p.y + seed);
	}

	inline Vec2 octave_noise_2d_to_2d(unsigned octaves, float persistence, Vec2 p, float seed = 0)
	{
		return {
			octave_noise_2d(octaves, persistence, p.x, p.y +  7 + seed),
			octave_noise_2d(octaves, persistence, p.x, p.y + 13 + seed),
		};
	}

	inline Vec3 octave_noise_2d_to_3d(unsigned octaves, float persistence, Vec2 p, float seed = 0)
	{
		return {
			octave_noise_2d(octaves, persistence, p.x, p.y +  7 + seed),
			octave_noise_2d(octaves, persistence, p.x, p.y + 13 + seed),
			octave_noise_2d(octaves, persistence, p.x, p.y + 17 + seed),
		};
	}

	inline Vec4 octave_noise_2d_to_4d(unsigned octaves, float persistence, Vec2 p, float seed = 0)
	{
		return {
			octave_noise_2d(octaves, persistence, p.x, p.y +  7 + seed),
			octave_noise_2d(octaves, persistence, p.x, p.y + 13 + seed),
			octave_noise_2d(octaves, persistence, p.x, p.y + 17 + seed),
			octave_noise_2d(octaves, persistence, p.x, p.y + 21 + seed),
		};
	}

	// ----------------------------------------------------------------------
	// 3d -> 2d, 3d, 4d:

	inline float octave_noise_3d_to_1d(unsigned octaves, float persistence, Vec3 p, float seed = 0)
	{
		return octave_noise_3d(octaves, persistence, p.x, p.y, p.z + seed);
	}

	inline Vec2 octave_noise_3d_to_2d(unsigned octaves, float persistence, Vec3 p, float seed = 0)
	{
		return {
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z +  7 + seed),
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z + 13 + seed),
		};
	}

	inline Vec3 octave_noise_3d_to_3d(unsigned octaves, float persistence, Vec3 p, float seed = 0)
	{
		return {
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z +  7 + seed),
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z + 13 + seed),
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z + 17 + seed),
		};
	}

	inline Vec4 octave_noise_3d_to_4d(unsigned octaves, float persistence, Vec3 p, float seed = 0)
	{
		return {
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z +  7 + seed),
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z + 13 + seed),
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z + 17 + seed),
			octave_noise_3d(octaves, persistence, p.x, p.y, p.z + 21 + seed),
		};
	}

	// ----------------------------------------------------------------------
	// 4d -> 2d, 3d, 4d:

	inline float octave_noise_4d_to_1d(unsigned octaves, float persistence, Vec4 p, float seed = 0)
	{
		return octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + seed);
	}

	inline Vec2 octave_noise_4d_to_2d(unsigned octaves, float persistence, Vec4 p, float seed = 0)
	{
		return {
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w +  7 + seed),
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + 13 + seed),
		};
	}

	inline Vec3 octave_noise_4d_to_3d(unsigned octaves, float persistence, Vec4 p, float seed = 0)
	{
		return {
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w +  7 + seed),
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + 13 + seed),
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + 17 + seed),
		};
	}

	inline Vec4 octave_noise_4d_to_4d(unsigned octaves, float persistence, Vec4 p, float seed = 0)
	{
		return {
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w +  7 + seed),
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + 13 + seed),
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + 17 + seed),
			octave_noise_4d(octaves, persistence, p.x, p.y, p.z, p.w + 21 + seed),
		};
	}
} // namespace emath
