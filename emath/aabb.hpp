 #pragma once

#include <cmath>
#include <initializer_list>

#include "vec2.hpp"
#include "mat3.hpp"

namespace emath {

template<typename T>
class AABB_T
{
public:
	using V = Vec2T<T>;
	using element_type = typename V::element_type; // e.g. 'float'
	static constexpr int DIM = 2;

	// Default ctor: NO INIT!
	AABB_T() { }

	// ------------------------------------------------
	// Static ctors:

	static const AABB_T from_points(std::initializer_list<V> list);
	static const AABB_T from_min_max(const V& min, const V& max);
	static const AABB_T from_min_size(const V& min, const V& size);
	static const AABB_T from_center_size(const V& center, const V& size);

	static const AABB_T everything();
	static const AABB_T nothing();

	// ------------------------------------------------

	const V& min()    const { return _min; }
	const V& max()    const { return _max; }
	const V  center() const { return average(_min, _max); }
	const V  size()   const { return _max-_min; }
	element_type area()   const { return size().area();   }
	element_type width()  const { return size().x; }
	element_type height() const { return size().y; }

	// Named in x-y order.
	const V  left_bottom()  const { return V(_min.x, _min.y); }
	const V  left_top()     const { return V(_min.x, _max.y); }
	const V  right_bottom() const { return V(_max.x, _min.y); }
	const V  right_top()    const { return V(_max.x, _max.y); }

	// ------------------------------------------------
	// Tests

	V clamp(const V& v) const
	{
		return emath::clamp(v, _min, _max);
	}

	bool contains(const V& v) const
	{
		for (int d=0; d<2; ++d) {
			if (v[d] < _min[d] || _max[d] < v[d]) {
				return false;
			}
		}
		return true;
	}

	bool contains(const AABB_T& b) const
	{
		return min().x <= b.min().x && b.max().x <= max().x
		    && min().y <= b.min().y && b.max().y <= max().y;
	}

	// <= 0 if inside.
	T distance_to(const V& v) const
	{
		if (contains(v)) {
			// Project v unto the closest of _min/_max on all dimensions
			V p = v;
			for (int i=0; i<DIM; ++i) {
				if (std::abs(_min[i]-v[i]) < std::abs(_max[i]-v[i])) {
					p[i] = _min[i];
				} else {
					p[i] = _max[i];
				}
			}

			// Now p is the closest corner
			return - std::min(std::abs(p[0] - v[0]), std::abs(p[1] - v[1]));
		} else {
			return distance(v, clamp(v));
		}
	}

	// ------------------------------------------------
	// Utils

	AABB_T enlarged_by_rad(T rad) const
	{
		V rv(rad);
		return from_min_max(min()-rv, max()+rv);
	}

	static bool intersects(const AABB_T& a, const AABB_T& b)
	{
		for (int d=0; d<2; ++d) {
			if (a._max[d] <= b._min[d] || b._max[d] <= a._min[d]) {
				return false;
			}
		}
		return true;
	}

	void include(const V& v)
	{
		for (int i=0; i<DIM; ++i) {
			_min[i] = emath::min(_min[i], v[i]);
			_max[i] = emath::max(_max[i], v[i]);
		}
	}

	void include(const AABB_T& b)
	{
		for (int i=0; i<DIM; ++i) {
			_min[i] = emath::min(_min[i], b._min[i]);
			_max[i] = emath::max(_max[i], b._max[i]);
		}
	}

	// ------------------------------------------------
	friend AABB_T operator*(T s, const AABB_T& v)
	{
		return AABB_T(s*v._min, s*v._max);
	}

	friend AABB_T operator*(const AABB_T& v, T s)
	{
		return AABB_T(v._min*s, v._max*s);
	}

private:
	AABB_T(const V& min, const V& max) : _min(min), _max(max) { }

	V _min, _max;
};

// ------------------------------------------------
using AABB  = AABB_T<real>;
using AABBf = AABB_T<float>;
using AABBd = AABB_T<double>;
using AABBi = AABB_T<int>;

// ------------------------------------------------
// Implementations

template<typename T>
inline const AABB_T<T> AABB_T<T>::from_points(std::initializer_list<V> list)
{
	assert(list.size()>0);
	auto data = list.begin();
	AABB_T ret = {data[0], data[0]};
	for (auto& v : list) {
		ret.include(v);
	}
	return ret;
}

template<typename T>
inline const AABB_T<T> AABB_T<T>::from_min_max(const V& min, const V& max)
{
	return AABB_T(min, max);
}

template<typename T>
inline const AABB_T<T> AABB_T<T>::from_min_size(const V& min, const V& size)
{
	return AABB_T(min, min+size);
}

template<typename T>
inline const AABB_T<T> AABB_T<T>::from_center_size(const V& center, const V& size)
{
	V hs = size/2;
	return AABB_T(center-hs, center+hs);
}

template<typename T>
inline const AABB_T<T> AABB_T<T>::everything()
{
	return from_min_max({-INF<T>, -INF<T>}, {+INF<T>, +INF<T>});
}

template<typename T>
inline const AABB_T<T> AABB_T<T>::nothing()
{
	return from_min_max({+INF<T>, +INF<T>}, {-INF<T>, -INF<T>});
}

// ------------------------------------------------

inline AABB lerp(const AABB& a, const AABB& b, float t)
{
	return AABB::from_center_size(
		lerp(a.center(), b.center(), t),
		lerp(a.size(),   b.size(),   t)
	);
}

template<typename T>
inline AABB_T<T> transform(const Mat3T<T>& out_from_in, const AABB_T<T>& aabb)
{
	return AABB_T<T>::from_points({
		mul_pos(out_from_in, Vec2T<T>{aabb.min().x, aabb.min().y}),
		mul_pos(out_from_in, Vec2T<T>{aabb.min().x, aabb.max().y}),
		mul_pos(out_from_in, Vec2T<T>{aabb.max().x, aabb.min().y}),
		mul_pos(out_from_in, Vec2T<T>{aabb.max().x, aabb.max().y}),
	});
}

} // namespace emath
