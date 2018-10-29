/*
 Adopted from Milk Engine for Life, 2007-03
 Adopted for EmiLib 2013-02
 */

#pragma once

#include "mat4.hpp"
#include "math.hpp"

namespace emath {

template <typename F>
class QuaternionT
{
private:
	using Vec3_ = Vec3T<F>;
	using Mat3_ = Mat3T<F>;

	union {
		struct { F q0, q1, q2, q3; };
		struct { F s;   Vec3_ v;   };
	};

public:
	// ------------------------------------------------

	QuaternionT() = default;  // Fast - no initialization!

	QuaternionT(const F _s, const Vec3_& _v) : s(_s), v(_v) {}

	QuaternionT(const F q0_, const F q1_, const F q2_, const F q3_)
		: q0(q0_), q1(q1_), q2(q2_), q3(q3_)
	{
	}

	// ------------------------------------------------

	const QuaternionT operator-() const { return QuaternionT(-s, -v); }

	const QuaternionT& operator+() const { return *this; }

	// ------------------------------------------------

	// template<class X>
	QuaternionT& operator*=(const F scalar)
	{
		s *= scalar;
		v *= scalar;
		return *this;
	}

	// template<>
	QuaternionT& operator*=(const QuaternionT& q)
	{
		const F a = (s   + v.x) * (q.s   + q.v.x), b = (v.z - v.y) * (q.v.y - q.v.z);
		const F c = (s   - v.x) * (q.v.y + q.v.z), d = (v.y + v.z) * (q.s   - q.v.x);
		const F e = (v.x + v.z) * (q.v.x + q.v.y), f = (v.x - v.z) * (q.v.x - q.v.y);
		const F g = (s   + v.y) * (q.s   - q.v.z), h = (s   - v.y) * (q.s   + q.v.z);

		s = b + (-e - f + g + h) / 2;
		v = Vec3_(a - (e + f + g + h) / 2, c + (e - f + g - h) / 2, d + (e - f - g + h) / 2);
		return *this;
	}

	friend const QuaternionT operator*(const QuaternionT& q1, const QuaternionT& q2)
	{
		const F a = (q1.s + q1.v.x) * (q2.s + q2.v.x), b = (q1.v.z - q1.v.y) * (q2.v.y - q2.v.z),
				c = (q1.s - q1.v.x) * (q2.v.y + q2.v.z), d = (q1.v.y + q1.v.z) * (q2.s - q2.v.x),
				e = (q1.v.x + q1.v.z) * (q2.v.x + q2.v.y),
				f = (q1.v.x - q1.v.z) * (q2.v.x - q2.v.y), g = (q1.s + q1.v.y) * (q2.s - q2.v.z),
				h = (q1.s - q1.v.y) * (q2.s + q2.v.z);

		return QuaternionT(b + (-e - f + g + h) / 2,
			Vec3_(a - (e + f + g + h) / 2, c + (e - f + g - h) / 2, d + (e - f - g + h) / 2));
	}

	friend const QuaternionT operator*(const QuaternionT& m, F s)
	{
		return QuaternionT(m.s * s, m.v * s);
	}

	friend const QuaternionT operator*(const F s, const QuaternionT& m)
	{
		return QuaternionT(m.s * s, m.v * s);
	}

	QuaternionT& operator/=(F s) { return *this *= 1 / s; }

	friend const QuaternionT operator/(const QuaternionT& m, const F s) { return m * (1 / s); }

	// ------------------------------------------------

	QuaternionT& operator+=(const QuaternionT& q)
	{
		s += q.s;
		v += q.v;
		return *this;
	}

	QuaternionT& operator-=(const QuaternionT& q)
	{
		s -= q.s;
		v -= q.v;
		return *this;
	}

	friend const QuaternionT operator+(const QuaternionT& q1, const QuaternionT& q2)
	{
		return QuaternionT(q1.s + q2.s, q1.v + q2.v);
	}

	friend const QuaternionT operator-(const QuaternionT& q1, const QuaternionT& q2)
	{
		return QuaternionT(q1.s - q2.s, q1.v - q2.v);
	}

	// ------------------------------------------------

	friend F dot(const QuaternionT& lhs, const QuaternionT& rhs)
	{
		return lhs.s * rhs.s + dot(lhs.v, rhs.v);
	}

	const F norm() const { return (s * s + dot(v, v)); }

	const F abs() const { return std::sqrt(norm()); }

	friend const QuaternionT conj(const QuaternionT& q) { return QuaternionT(q.s, -q.v); }

	friend const QuaternionT inverse(const QuaternionT& q) { return conj(q) / q.norm(); }

	void normalize()
	{
		F inv_len = 1 / std::sqrt(norm());
		s *= inv_len;
		v *= inv_len;
	}

	bool is_normalized() const
	{
		return is_zero(norm() - 1);
	}

	// ------------------------------------------------
	// Constructors.

	static QuaternionT identity() { return QuaternionT(1, Vec3_()); }

	static const QuaternionT from_axis(const Vec3_& axis, F radians)
	{
		assert(emath::is_normalized(axis));
		return QuaternionT(std::cos(radians / 2), axis * std::sin(radians / 2));
	}

	static const QuaternionT from_axis(const Vec3_& axis)
	{
		F len = length(axis);
		if (is_zero(len)) {
			return identity();
		}
		return from_axis(axis / len, len);
	}

	static const QuaternionT from_euler(const F x, const F y, const F z)
	{
		// Straight from wikipedia.
		// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		F cx = std::cos(x / 2);
		F cy = std::cos(y / 2);
		F cz = std::cos(z / 2);
		F sx = std::sin(x / 2);
		F sy = std::sin(y / 2);
		F sz = std::sin(z / 2);

		return {
			cx * cy * cz + sx * sy * sz,
			sx * cy * cz - cx * sy * sz,
			cx * sy * cz + sx * cy * sz,
			cx * cy * sz - sx * sy * cz,
		};
	}

	static const QuaternionT from_euler(const Vec3_& euler)
	{
		return from_euler(euler.x, euler.y, euler.z);
	}

	static const QuaternionT from_matrix(const Mat3_& m)
	{
		return from_matrix_transposed(transposed(m));
	}

	static const QuaternionT from_matrix_transposed(const Mat3_& m)
	{
		const F epsilon = 0.00001f;
		F trace = m.M(0, 0) + m.M(1, 1) + m.M(2, 2) + (F)1.0;

		F s;
		Vec3_ v;

		if (trace > epsilon) {
			const F s_ = F(0.5) / std::sqrt(trace);
			s = F(0.25) / s_;
			v.x = (m.M(1, 2) - m.M(2, 1)) * s_;
			v.y = (m.M(2, 0) - m.M(0, 2)) * s_;
			v.z = (m.M(0, 1) - m.M(1, 0)) * s_;
		} else {
			if (m.M(0, 0) > m.M(1, 1) && m.M(0, 0) > m.M(2, 2)) {
				const F s_ = F(2) * std::sqrt(1.0f + m.M(0, 0) - m.M(1, 1) - m.M(2, 2));
				v.x = F(0.25) * s_;
				v.y = (m.M(1, 0) + m.M(0, 1)) / s_;
				v.z = (m.M(2, 0) + m.M(0, 2)) / s_;
				s = (m.M(2, 1) - m.M(1, 2)) / s_;
			} else if (m.M(1, 1) > m.M(2, 2)) {
				const F s_ = F(2) * std::sqrt(1.0f + m.M(1, 1) - m.M(0, 0) - m.M(2, 2));
				v.x = (m.M(1, 0) + m.M(0, 1)) / s_;
				v.y = F(0.25) * s_;
				v.z = (m.M(2, 1) + m.M(1, 2)) / s_;
				s = (m.M(2, 0) - m.M(0, 2)) / s_;
			} else {
				const F s_ = F(2) * std::sqrt(1.0f + m.M(2, 2) - m.M(0, 0) - m.M(1, 1));
				v.x = (m.M(2, 0) + m.M(0, 2)) / s_;
				v.y = (m.M(2, 1) + m.M(1, 2)) / s_;
				v.z = F(0.25) * s_;
				s = (m.M(1, 0) - m.M(0, 1)) / s_;
			}
		}
		return QuaternionT(s, v);
	}

	// GETTERS

	const Vec3_ euler() const
	{
		// Straight from wikipedia.
		// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		return {
			std::atan2(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1 * q1 + q2 * q2)),
			std::asin(2 * (q0 * q2 - q3 * q1)),
			std::atan2(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2 * q2 + q3 * q3))
		};
	}

	void axis_angle(Vec3_& axis, F& angle) const
	{
		angle = 2 * std::acos(s);
		F s2 = std::sqrt(1 - s * s);
		if (is_zero(s2)) {
			s2 = 1;
		}
		axis = v / s2;
	}

	const Mat3_ as_mat3() const
	{
		// return transposed( as_mat3_transposed() );
		return as_mat3_transposed();  // FIXME: this or from_matrix is wrong
	}

	const Mat3_ as_mat3_transposed() const
	{
		assert(is_normalized());

		const F x2 = v.x * v.x;
		const F y2 = v.y * v.y;
		const F z2 = v.z * v.z;

		return Mat3_(
			1 - 2*y2 - 2*z2,      2*v.x*v.y - 2*v.z*s,  2*v.x*v.z + 2*v.y*s,
			2*v.x*v.y + 2*v.z*s,  1 - 2*x2 - 2*z2,      2*v.y*v.z - 2*v.x*s,
			2*v.x*v.z - 2*v.y*s,  2*v.y*v.z + 2*v.x*s,  1 - 2*x2 - 2*y2);
	}

	// ------------------------------------------------

	friend const Vec3_ transform(const QuaternionT& q, const Vec3_& v)
	{
		return ((q * QuaternionT(0, v)) * inverse(q)).v;
	}
};

/* gets the rotation quaternion for rotating
 from v1 to v2. v1 and v2 should be normalized! */
template <class F>
QuaternionT<F> roataion_arc(const Vec3T<F>& v0, const Vec3T<F>& v1)
{
	F s = 1.0f / std::sqrt(2 + 2 * dot(v0, v1));
	return QuaternionT<F>(1 / (2 * s), cross(v0, v1) * s);
}

/* spherical linear quaternion interpolation */
template <class F>
QuaternionT<F> slerp(const QuaternionT<F>& q0, const QuaternionT<F>& q1, F t)
{
	F d = dot(q0, q1);
	QuaternionT<F> q2;

	/*
	 dot = cos(theta)
	 if (dot < 0), q0 and q1 are more than 90 degrees apart,
	 so we can invert one to reduce spinning
	 */

	// remove this for squad
	if (d < 0) {
		d = -d;
		q2 = -1.0f * q1;
	} else {
		q2 = q1;
	}

	if (d < 0.95f) {
		F angle = std::acos(d);
		F sina = std::sin(angle);
		F sinat = std::sin(angle * t);
		F sinaomt = std::sin(angle * (1 - t));
		return (q0 * sinaomt + q2 * sinat) / sina;
	} else {
		return lerp(q0, q2, t);  // use linear interpolation on small angles
	}
}

/// simpler version of the above for splerp
template <class F>
QuaternionT<F> simple_slerp(const QuaternionT<F>& q0, const QuaternionT<F>& q1, F t)
{
	F d = dot(q0, q1);

	if (-0.95f < d && d < +0.95f) {
		F angle   = std::acos(d);
		F sina    = std::sin(angle);
		F sinat   = std::sin(angle *      t);
		F sinaomt = std::sin(angle * (1 - t));
		return (q0 * sinaomt + q1 * sinat) / sina;
	} else {
		return lerp(q0, q1, t);  // use linear interpolation on small angles
	}
}

/* Spherical cubic spline quaternion interpolation'
 q0->q1 is the distance to travel, a and b are control-
 quaternions, so in sequence: a, q1, q2, b */
template <class F>
inline QuaternionT<F> splerp(const QuaternionT<F>& q0, const QuaternionT<F>& q1,
	const QuaternionT<F>& a, const QuaternionT<F>& b, F t)
{
	return simple_slerp(2 * t * (1 - t), simple_slerp(t, q0, q1), simple_slerp(t, a, b));
}

/* Linear interpolation between matrices with rotation and translation.
 Suitable for view-matrices (i.e. Mat4::look_at)
 */
template <class X>
inline Mat4T<X> view_matrix_lerp(const Mat4T<X>& m0, const Mat4T<X>& m1, X t)
{
	// Translation in world space:
	auto trans0_w = mul(inverted(m0.upper_left3x3()), m0.translation());
	auto trans1_w = mul(inverted(m1.upper_left3x3()), m1.translation());
	auto trans_t_w = lerp(trans0_w, trans1_w, t);

	auto q0 = QuaternionT<X>::from_matrix(m0.upper_left3x3());
	q0.normalize();
	auto q1 = QuaternionT<X>::from_matrix(m1.upper_left3x3());
	q1.normalize();
	auto qt = slerp(q0, q1, t);
	qt.normalize();

	auto rot_mat3 = qt.as_mat3();
	auto rot_mat4 = Mat4T<X>::from_rot_mat(rot_mat3);
	auto trans_t = mul(rot_mat3, trans_t_w);

	auto mat = rot_mat4;
	mat.set_translation(trans_t);
	return mat;
}

using Quatf = QuaternionT<float>;
using Quatd = QuaternionT<double>;

static_assert(sizeof(Quatf) == 4 * sizeof(float), "Pack");
static_assert(sizeof(Quatd) == 4 * sizeof(double), "Pack");

static_assert(std::is_pod<Quatf>::value,                "is_pod");
static_assert(std::is_standard_layout<Quatf>::value,    "is_standard_layout");
static_assert(std::is_trivial<Quatf>::value,            "is_trivial");
static_assert(std::is_trivially_copyable<Quatf>::value, "is_trivially_copyable");

} // namespace emath
