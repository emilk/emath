// Version 1.1
#pragma once

#include <algorithm>

#include "fwd.hpp"
#include "mat3.hpp"
#include "vec3.hpp"

namespace emath
{
	/*
	 Same memory order as OpenGL.
	 Translation is in bottom row, at {[3][0], [3][1], [3][2]}
	 This is at {data[12], data[13], data[14]}
	 We still multiply vectors from the right.
	 This is slightly peculiar, as that does not conform to column vectors.
	 */
	template<typename T>
	class Mat4T
	{
	public:
		T mat[4][4];

	public:
		Mat4T() = default; // Fast

		inline Mat4T(const Mat4T& m);
		template<typename T2>

		// Explcit cast:
		inline explicit Mat4T(const Mat4T<T2>& m) {
			for (int j = 0; j < 4; ++j) {
				for (int i = 0; i < 4; ++i) {
					mat[i][j] = static_cast<T>( m[i][j] );
				}
			}
		}

		inline explicit Mat4T(const Mat3& m);

		inline explicit Mat4T(const T* d);
		inline Mat4T(T m00, T m01, T m02, T m03,
		             T m10, T m11, T m12, T m13,
		             T m20, T m21, T m22, T m23,
		             T m30, T m31, T m32, T m33);

		// ------------------------------------------------
		// Inspectors

		inline       T* data() { return &mat[0][0]; }
		inline const T* data() const { return &mat[0][0]; }

		Vec4 row(uint num) const
		{
			return Vec4(mat[num][0], mat[num][1], mat[num][2], mat[num][3]);
		}

		Vec4 col(uint num) const
		{
			return Vec4(mat[0][num], mat[1][num], mat[2][num], mat[3][num]);
		}

		inline       T*  operator [] (int row);
		inline const T*  operator [] (int row) const;
		inline const Mat4T&   operator =  (const Mat4T& rhs);
		inline       Mat4T    operator *  (const Mat4T& rhs) const;
		inline const Mat4T&   operator *= (const Mat4T& rhs);
		inline       Mat4T    operator +  (const Mat4T& rhs) const;
		inline const Mat4T&   operator += (const Mat4T& rhs);
		inline       Mat4T    operator *  (T s) const;
		inline       Mat4T    operator /  (T s) const { return *this * (1/s); }
		inline       bool     operator == (const Mat4T& rhs) const;

		// Get the translation part of the matrix
		inline Vec3 translation() const;
		inline void set_translation_2d(const Vec2& v);
		inline void set_translation(const Vec3& v);

		inline Vec3 scaling3() const;
		inline float scaling() const;

		inline Vec2  dir_z() const;
		inline float angle_z() const;

		inline Mat3 upper_left3x3() const
		{
			return Mat3(
				mat[0][0], mat[0][1], mat[0][2],
				mat[1][0], mat[1][1], mat[1][2],
				mat[2][0], mat[2][1], mat[2][2]);
		}

		// Use the returned matrix to transform normals (directions)
		inline Mat3 normal_transformer() const
		{
			return transposed(inverted(upper_left3x3()));
		}

		// The adjoint is the conjugate transpose of the matrix.
		inline Mat4T  adjoint() const;
		inline T      determinant() const;
		inline Mat4T  ortho_inverse() const;

		// ------------------------------------------------
		// Static initializers:

		inline static Mat4T zero();
		inline static Mat4T identity();
		inline static Mat4T translate_2d(const Vec2T<T>& v);
		inline static Mat4T translate(const Vec3T<T>& v);
		inline static Mat4T translate(T x, T y, T z);
		inline static Mat4T rotate_x(T rad);
		inline static Mat4T rotate_y(T rad);
		inline static Mat4T rotate_z(T rad);
		inline static Mat4T rotate(Vec3T<T> rad);
		inline static Mat4T rotate_around_2d(Vec2T<T> point, float rad);
		inline static Mat4T scale(T x, T y, T z);
		inline static Mat4T scale(T u) { return scale(u,u,u); }
		inline static Mat4T scale_2d(const Vec2T<T>& v) { return scale(v.x, v.y, 1); }
		inline static Mat4T scale_2d(float u) { return scale(u,u,1); }
		inline static Mat4T perspective(T fov_y, T aspect_ratio, T zNear, T zFar);

		inline static Mat4T look_at(const Vec3T<T>& eye, const Vec3T<T>& ref, const Vec3T<T>& up);

		inline static Mat4T ortho(const Vec2T<T>& size, bool y_increases_down);

		inline static Mat4T transform(const Vec3T<T>& pos, const Vec3T<T>& rot);

		inline static Mat4T from_rot_mat(const Mat3T<T>& m);
		inline static Mat4T from_mat_2d(const Mat3T<T>& m);

		inline static Mat4T from_pos_angle_2d(Vec2T<T> translation, float angle_z) {
			return Mat4T::translate_2d(translation) * Mat4T::rotate_z(angle_z);
		}
	};

	template<typename T>
	std::ostream& operator << (std::ostream& os, const Mat4T<T>& mat);

	template<typename T>
	inline Mat4T<T> transposed(const Mat4T<T>& arg);

	template<typename T>
	inline Mat4T<T> inverted(const Mat4T<T>& arg);

	// ------------------------------------------------
	// Implementation:

	template<typename T>
	inline Mat4T<T>::Mat4T(const Mat4T& m)
	{
		for (int j = 0; j < 4; ++j) {
			for (int i = 0; i < 4; ++i) {
				mat[i][j] = m[i][j];
			}
		}
	}

	template<typename T>
	inline Mat4T<T>::Mat4T(const T* d)
	{
		int index = 0;
		for (int j = 0; j < 4; ++j) {
			for (int i = 0; i < 4; ++i) {
				mat[i][j] = d[index++];
			}
		}
	}

	template<typename T>
	inline Mat4T<T>::Mat4T(T m00, T m01, T m02, T m03,
							T m10, T m11, T m12, T m13,
							T m20, T m21, T m22, T m23,
							T m30, T m31, T m32, T m33)
	{
		mat[0][0] = m00;	mat[0][1] = m01;	mat[0][2] = m02;	mat[0][3] = m03;
		mat[1][0] = m10;	mat[1][1] = m11;	mat[1][2] = m12;	mat[1][3] = m13;
		mat[2][0] = m20;	mat[2][1] = m21;	mat[2][2] = m22;	mat[2][3] = m23;
		mat[3][0] = m30;	mat[3][1] = m31;	mat[3][2] = m32;	mat[3][3] = m33;
	}

	template<typename T>
	inline T* Mat4T<T>::operator [] (int row)
	{
		return &mat[row][0];
	}

	template<typename T>
	inline const T* Mat4T<T>::operator [] (int row) const
	{
		return &mat[row][0];
	}

	template<typename T>
	inline const Mat4T<T>& Mat4T<T>::operator = (const Mat4T& rhs)
	{
		for (int j = 0; j < 4; ++j) {
			for (int i = 0; i < 4; ++i) {
				mat[i][j] = rhs[i][j];
			}
		}

		return *this;
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::operator *  (const Mat4T& rhs) const
	{
		const Mat4T& B = *this;
		const Mat4T& A = rhs;
		Mat4T M = Mat4T<T>::zero();

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					M[i][j] += A[i][k] * B[k][j];
				}
			}
		}

		return M;
	}

	template<typename T>
	inline const Mat4T<T>& Mat4T<T>::operator *= (const Mat4T& rhs)
	{
		*this = *this * rhs;

		return *this;
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::operator +  (const Mat4T& rhs) const
	{
		Mat4T result(*this);

		for (int j = 0; j < 4; ++j) {
			for (int i = 0; i < 4; ++i) {
				result[i][j] += rhs[i][j];
			}
		}

		return result;
	}

	template<typename T>
	inline const Mat4T<T>& Mat4T<T>::operator += (const Mat4T& rhs)
	{
		for (int j = 0; j < 4; ++j) {
			for (int i = 0; i < 4; ++i) {
				mat[i][j] += rhs[i][j];
			}
		}

		return *this;
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::operator * (T s) const
	{
		Mat4T m;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m.mat[i][j] = mat[i][j] * s;
			}
		}

		return m;
	}

	template<typename T>
	inline bool Mat4T<T>::operator == (const Mat4T<T>& rhs) const
	{
		for (int j = 0; j < 4; ++j) {
			for (int i = 0; i < 4; ++i) {
				if (mat[i][j] != rhs.mat[i][j]) {
					return false;
				}
			}
		}
		return true;
	}

	// Get the translation part of the matrix
	template<typename T>
	inline Vec3 Mat4T<T>::translation() const
	{
		return { mat[3][0], mat[3][1], mat[3][2] };
	}

	// Get the Scaling part of the matrix
	template<typename T>
	inline Vec3 Mat4T<T>::scaling3() const
	{
		// TODO: or is it row?
		return {
			length(col(0).xyz),
			length(col(1).xyz),
			length(col(2).xyz),
		};
	}

	/// The signed uniform scaling of the matrix.
	template<typename T>
	inline float Mat4T<T>::scaling() const
	{
		return std::cbrt(upper_left3x3().determinant());
	}

	template<typename T>
	inline Vec2 Mat4T<T>::dir_z() const
	{
		return row(0).xy;
	}

	template<typename T>
	inline float Mat4T<T>::angle_z() const
	{
		return emath::angle( dir_z() );
	}

	template<typename T>
	inline void Mat4T<T>::set_translation_2d(const Vec2& v)
	{
		mat[3][0] = (T)v.x;
		mat[3][1] = (T)v.y;
	}

	template<typename T>
	inline void Mat4T<T>::set_translation(const Vec3& v)
	{
		mat[3][0] = (T)v.x;
		mat[3][1] = (T)v.y;
		mat[3][2] = (T)v.z;
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::zero()
	{
		return Mat4T(
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::identity()
	{
		return Mat4T(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);
	}

	// ------------------------------------------------
	// Static ctors

	template<typename T>
	inline Mat4T<T> Mat4T<T>::translate_2d(const Vec2T<T>& v)
	{
		return translate((T)v.x, (T)v.y, 0);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::translate(const Vec3T<T>& v)
	{
		return translate((T)v.x, (T)v.y, (T)v.z);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::translate(T x, T y, T z)
	{
		return Mat4T(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			x,y,z,1);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::rotate_x(T rad)
	{
		T c = std::cos(rad);
		T s = std::sin(rad);

		return Mat4T(
			1, 0, 0, 0,
			0, c, s, 0,
			0,-s, c, 0,
			0, 0, 0, 1);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::rotate_y(T rad)
	{
		T c = std::cos(rad);
		T s = std::sin(rad);
		return Mat4T(
			c, 0, -s, 0,
			0, 1, 0,  0,
			s, 0, c,  0,
			0, 0, 0,  1);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::rotate_z(T rad)
	{
		T c = std::cos(rad);
		T s = std::sin(rad);
		return Mat4T(
			c,  s, 0, 0,
			-s, c, 0, 0,
			0,  0, 1, 0,
			0,  0, 0, 1);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::rotate(Vec3T<T> rad)
	{
		const auto cx = std::cos(rad.x), sx = std::sin(rad.x);
		const auto cy = std::cos(rad.y), sy = std::sin(rad.y);
		const auto cz = std::cos(rad.z), sz = std::sin(rad.z);
		return transposed( Mat4T{
			cz*cy,   (cz*sy*sx)-(sz*cx),   (sz*sx)+(cz*sy*cx),   0,
			sz*cy,   (cz*cx)+(sz*sy*sx),   (sz*sy*cx)-(cz*sx),   0,
			-sy,     cy*sx,                cy*cx,                0,
			0,       0,                    0,                    1
		} );
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::rotate_around_2d(Vec2T<T> point, float rad)
	{
		return Mat4T<T>::translate_2d(point) * Mat4T<T>::rotate_z(rad) * Mat4T<T>::translate_2d(-point);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::scale(T x, T y, T z)
	{
		return Mat4T(
			x,0,0,0,
			0,y,0,0,
			0,0,z,0,
			0,0,0,1);
	}

#if 0
	template<typename T>
	inline Mat4T<T> Mat4T<T>::perspective(T fov_y_deg, T aspect_ratio, T zNear, T zFar)
	{
		T half_h = std::tan((3.141592f/180.0f)*(fov_y_deg/2.0f))*zNear;
		T half_w = half_h * aspect_ratio;

		T l = -half_w;
		T r = half_w;
		T t = half_h;
		T b = -half_h;
		T f = zFar;
		T n = zNear;

		T A = (r+l)/(r-l);
		T B = (t+b)/(t-b);
		T C = -(f+n)/(f-n);
		T D = -2*f*n/(f-n);
		T P = 2*n/(r-l);
		T Q = 2*n/(t-b);

		if (!std::isfinite(f)) {
			// The limit of an infinite far plane is:
			C = -1;
			D = -2 * n;
		}

		return Mat4T(
			P, 0, 0, 0,
			0, Q, 0, 0,
			A, B, C,-1,
			0, 0, D, 0);
	}

#else
	// Create a perspective projection matrix.
	// Same arguments as to gluPerspective.
	// aspect = w/h
	template<typename T>
	inline Mat4T<T> Mat4T<T>::perspective(T fov_yin_deg, T aspect, T n, T f)
	{
		T top = n * std::tan(emath::deg2rad(fov_yin_deg) / 2);
		T right = top * aspect;

		return Mat4T(
			n/right,   0,       0,              0,
			0,         n/top,   0,              0,
			0,         0,       -(f+n)/(f-n),   -1,
			0,         0,       -2*f*n/(f-n),   0);
	}
#endif

	template<typename T>
	inline Mat4T<T> Mat4T<T>::look_at(const Vec3T<T>& eye, const Vec3T<T>& ref, const Vec3T<T>& up)
	{
		auto zaxis = eye - ref; normalize(zaxis);
		auto xaxis = cross(up, zaxis); normalize(xaxis);
		auto yaxis = cross(zaxis, xaxis);

		return Mat4T(
			xaxis.x,          yaxis.x,          zaxis.x,          0,
			xaxis.y,          yaxis.y,          zaxis.y,          0,
			xaxis.z,          yaxis.z,          zaxis.z,          0,
			-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1);
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::ortho(const Vec2T<T>& size, bool y_increases_down)
	{
		const T y_sign = (y_increases_down ? -1 : +1);
		Mat4T mat = Mat4T<T>::translate(-1.0f, y_sign * -1.0f, 0.0f);
		mat *= Mat4T<T>::scale(2.0f/size.x, y_sign * 2.0f/size.y, 1.0f);
		return mat;
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::transform(const Vec3T<T>& pos, const Vec3T<T>& rot)
	{
		Mat4T m = rotate(rot);
		m.set_translation(pos);
		return m;
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::from_rot_mat(const Mat3T<T>& m)
	{
		return {
			m.M(0,0), m.M(0,1), m.M(0,2), 0,
			m.M(1,0), m.M(1,1), m.M(1,2), 0,
			m.M(2,0), m.M(2,1), m.M(2,2), 0,
			0,        0,        0,        1
		};
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::from_mat_2d(const Mat3T<T>& m) {
		return {
			m.M(0,0), m.M(0,1), 0,  m.M(0,2),
			m.M(1,0), m.M(1,1), 0,  m.M(1,2),
			0,        0,        1,         0,
			m.M(2,0), m.M(2,1), 0,  m.M(2,2),
		};
	}

	// ------------------------------------------------

	// The adjoint (conjugate transpose) of the matrix.
	template<typename T>
	inline Mat4T<T> Mat4T<T>::adjoint() const
	{
		return Mat4T(
			mat[1][2]*mat[2][3]*mat[3][1] - mat[1][3]*mat[2][2]*mat[3][1] + mat[1][3]*mat[2][1]*mat[3][2] - mat[1][1]*mat[2][3]*mat[3][2] - mat[1][2]*mat[2][1]*mat[3][3] + mat[1][1]*mat[2][2]*mat[3][3],
			mat[0][3]*mat[2][2]*mat[3][1] - mat[0][2]*mat[2][3]*mat[3][1] - mat[0][3]*mat[2][1]*mat[3][2] + mat[0][1]*mat[2][3]*mat[3][2] + mat[0][2]*mat[2][1]*mat[3][3] - mat[0][1]*mat[2][2]*mat[3][3],
			mat[0][2]*mat[1][3]*mat[3][1] - mat[0][3]*mat[1][2]*mat[3][1] + mat[0][3]*mat[1][1]*mat[3][2] - mat[0][1]*mat[1][3]*mat[3][2] - mat[0][2]*mat[1][1]*mat[3][3] + mat[0][1]*mat[1][2]*mat[3][3],
			mat[0][3]*mat[1][2]*mat[2][1] - mat[0][2]*mat[1][3]*mat[2][1] - mat[0][3]*mat[1][1]*mat[2][2] + mat[0][1]*mat[1][3]*mat[2][2] + mat[0][2]*mat[1][1]*mat[2][3] - mat[0][1]*mat[1][2]*mat[2][3],

			mat[1][3]*mat[2][2]*mat[3][0] - mat[1][2]*mat[2][3]*mat[3][0] - mat[1][3]*mat[2][0]*mat[3][2] + mat[1][0]*mat[2][3]*mat[3][2] + mat[1][2]*mat[2][0]*mat[3][3] - mat[1][0]*mat[2][2]*mat[3][3],
			mat[0][2]*mat[2][3]*mat[3][0] - mat[0][3]*mat[2][2]*mat[3][0] + mat[0][3]*mat[2][0]*mat[3][2] - mat[0][0]*mat[2][3]*mat[3][2] - mat[0][2]*mat[2][0]*mat[3][3] + mat[0][0]*mat[2][2]*mat[3][3],
			mat[0][3]*mat[1][2]*mat[3][0] - mat[0][2]*mat[1][3]*mat[3][0] - mat[0][3]*mat[1][0]*mat[3][2] + mat[0][0]*mat[1][3]*mat[3][2] + mat[0][2]*mat[1][0]*mat[3][3] - mat[0][0]*mat[1][2]*mat[3][3],
			mat[0][2]*mat[1][3]*mat[2][0] - mat[0][3]*mat[1][2]*mat[2][0] + mat[0][3]*mat[1][0]*mat[2][2] - mat[0][0]*mat[1][3]*mat[2][2] - mat[0][2]*mat[1][0]*mat[2][3] + mat[0][0]*mat[1][2]*mat[2][3],

			mat[1][1]*mat[2][3]*mat[3][0] - mat[1][3]*mat[2][1]*mat[3][0] + mat[1][3]*mat[2][0]*mat[3][1] - mat[1][0]*mat[2][3]*mat[3][1] - mat[1][1]*mat[2][0]*mat[3][3] + mat[1][0]*mat[2][1]*mat[3][3],
			mat[0][3]*mat[2][1]*mat[3][0] - mat[0][1]*mat[2][3]*mat[3][0] - mat[0][3]*mat[2][0]*mat[3][1] + mat[0][0]*mat[2][3]*mat[3][1] + mat[0][1]*mat[2][0]*mat[3][3] - mat[0][0]*mat[2][1]*mat[3][3],
			mat[0][1]*mat[1][3]*mat[3][0] - mat[0][3]*mat[1][1]*mat[3][0] + mat[0][3]*mat[1][0]*mat[3][1] - mat[0][0]*mat[1][3]*mat[3][1] - mat[0][1]*mat[1][0]*mat[3][3] + mat[0][0]*mat[1][1]*mat[3][3],
			mat[0][3]*mat[1][1]*mat[2][0] - mat[0][1]*mat[1][3]*mat[2][0] - mat[0][3]*mat[1][0]*mat[2][1] + mat[0][0]*mat[1][3]*mat[2][1] + mat[0][1]*mat[1][0]*mat[2][3] - mat[0][0]*mat[1][1]*mat[2][3],

			mat[1][2]*mat[2][1]*mat[3][0] - mat[1][1]*mat[2][2]*mat[3][0] - mat[1][2]*mat[2][0]*mat[3][1] + mat[1][0]*mat[2][2]*mat[3][1] + mat[1][1]*mat[2][0]*mat[3][2] - mat[1][0]*mat[2][1]*mat[3][2],
			mat[0][1]*mat[2][2]*mat[3][0] - mat[0][2]*mat[2][1]*mat[3][0] + mat[0][2]*mat[2][0]*mat[3][1] - mat[0][0]*mat[2][2]*mat[3][1] - mat[0][1]*mat[2][0]*mat[3][2] + mat[0][0]*mat[2][1]*mat[3][2],
			mat[0][2]*mat[1][1]*mat[3][0] - mat[0][1]*mat[1][2]*mat[3][0] - mat[0][2]*mat[1][0]*mat[3][1] + mat[0][0]*mat[1][2]*mat[3][1] + mat[0][1]*mat[1][0]*mat[3][2] - mat[0][0]*mat[1][1]*mat[3][2],
			mat[0][1]*mat[1][2]*mat[2][0] - mat[0][2]*mat[1][1]*mat[2][0] + mat[0][2]*mat[1][0]*mat[2][1] - mat[0][0]*mat[1][2]*mat[2][1] - mat[0][1]*mat[1][0]*mat[2][2] + mat[0][0]*mat[1][1]*mat[2][2]);
	}

	template<typename T>
	inline T Mat4T<T>::determinant() const
	{
		const auto
		a = mat[2][2]*mat[3][3] - mat[2][3]*mat[3][2],
		b = mat[2][1]*mat[3][3] - mat[2][3]*mat[3][1],
		c = mat[2][1]*mat[3][2] - mat[2][2]*mat[3][1],
		d = mat[2][0]*mat[3][3] - mat[2][3]*mat[3][0],
		e = mat[2][0]*mat[3][2] - mat[2][2]*mat[3][0],
		f = mat[2][0]*mat[3][1] - mat[2][1]*mat[3][0];

		return (
			+mat[0][0]*(mat[1][1]*a - mat[1][2]*b + mat[1][3]*c)
			-mat[0][1]*(mat[1][0]*a - mat[1][2]*d + mat[1][3]*e)
			+mat[0][2]*(mat[1][0]*b - mat[1][1]*d + mat[1][3]*f)
			-mat[0][3]*(mat[1][0]*c - mat[1][1]*e + mat[1][2]*f));
	}

	template<typename T>
	inline Mat4T<T> Mat4T<T>::ortho_inverse() const
	{
		return Mat4T(
			+mat[0][0], +mat[1][0], +mat[2][0], +mat[0][3],
			+mat[0][1], +mat[1][1], +mat[2][1], +mat[1][3],
			+mat[0][2], +mat[1][2], +mat[2][2], +mat[2][3],
			-mat[3][0], -mat[3][1], -mat[3][2], +mat[3][3]);
	}

	template<typename T>
	inline Mat4T<T> transposed(const Mat4T<T>& arg) {
		auto& m = arg.mat;

		return Mat4T<T>{
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3],
		};
	}

	template<typename T>
	inline Mat4T<T> inverted(const Mat4T<T>& m)
	{
		return m.adjoint() / m.determinant();
	}

	template<typename T>
	inline Vec4T<T> mul(const Mat4T<T>& m, const Vec4T<T>& p)
	{
		return Vec4(
			m.mat[0][0]*p[0] + m.mat[1][0]*p[1] + m.mat[2][0]*p[2] + m.mat[3][0]*p[3],
			m.mat[0][1]*p[0] + m.mat[1][1]*p[1] + m.mat[2][1]*p[2] + m.mat[3][1]*p[3],
			m.mat[0][2]*p[0] + m.mat[1][2]*p[1] + m.mat[2][2]*p[2] + m.mat[3][2]*p[3],
			m.mat[0][3]*p[0] + m.mat[1][3]*p[1] + m.mat[2][3]*p[2] + m.mat[3][3]*p[3]);
	}

	template<typename T>
	inline Vec2f mul_pos(const Mat4T<T>& m, const Vec2f& p)
	{
		auto v4 = mul(m, Vec4T<T>(Vec4f(p, 0, 1)));
		return Vec2f( v4.xy / v4.w );
	}

	template<typename T>
	inline Vec2d mul_pos(const Mat4T<T>& m, const Vec2d& p)
	{
		auto v4 = mul(m, Vec4T<T>(Vec4d(p, 0, 1)));
		return v4.xy / v4.w;
	}

	template<typename T>
	inline Vec3f mul_pos(const Mat4T<T>& m, const Vec3f& p)
	{
		auto v4 = mul(m, Vec4T<T>(Vec4f(p, 1)));
		return Vec3f( v4.xyz / v4.w );
	}

	template<typename T>
	inline Vec3d mul_pos(const Mat4T<T>& m, const Vec3d& p)
	{
		auto v4 = mul(m, Vec4T<T>(Vec4d(p, 1)));
		return v4.xyz / v4.w;
	}

	template<typename T>
	inline Vec3T<T> mul_dir(const Mat4T<T>& m, const Vec3T<T>& dir)
	{
		return mul(m.normal_transformer(), dir);
	}

	template<typename T>
	inline Vec2T<T> mul_dir(const Mat4T<T>& m, const Vec2T<T>& dir)
	{
		return mul(m.normal_transformer(), Vec3T<T>(dir, 0)).xy;
	}

	// ------------------------------------------------

	using Mat4  = Mat4T<real>;
	using Mat4f = Mat4T<float>;
	using Mat4d = Mat4T<double>;
}
