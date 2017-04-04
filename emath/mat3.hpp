#pragma once

#include <loguru.hpp>

#include "vec2.hpp"
#include "vec3.hpp"

namespace emath
{
	using uint = unsigned;

	/*
	 For discussions about memory order and the like, see Mat4
	 */
	template<typename T>
	class Mat3T
	{
	public:
		T mat[3][3];

	public:
		Mat3T() = default; // Fast

		explicit inline Mat3T(const T* d);
		inline Mat3T(T m00, T m01, T m02,
						 T m10, T m11, T m12,
						 T m20, T m21, T m22);

		inline T& M(uint row, uint col)       { return mat[row][col]; }
		inline T  M(uint row, uint col) const { return mat[row][col]; }

		inline       T* data()       { return &mat[0][0]; }
		inline const T* data() const { return &mat[0][0]; }

		inline Mat3T inverse() const;

		inline T*             operator [] (int row);
		inline const T*       operator [] (int row) const;
		inline Mat3T          operator *  (const Mat3T& rhs) const;
		inline const Mat3T&   operator *= (const Mat3T& rhs);
		inline Mat3T          operator +  (const Mat3T& rhs) const;
		inline const Mat3T&   operator += (const Mat3T& rhs);
		inline Mat3T          operator * (T s);
		inline const Mat3T&   operator *= (T s);
		inline friend bool    operator == (const Mat3T& a, const Mat3T& b);

		// ------------------------------------------------
		// Static constructors

		inline static Mat3T translate(T x, T y);
		inline static Mat3T translate(const emath::Vec2& v) { return translate((T)v.x, (T)v.y); }
		inline static Mat3T rotate_x(T rad);
		inline static Mat3T rotate_y(T rad);
		inline static Mat3T rotate_z(T rad);
		inline static Mat3T scale(T x, T y);
		inline static Mat3T scale(const emath::Vec2& v) { return scale((T)v.x, (T)v.y); }

		inline static Mat3T zero();
		inline static Mat3T identity();

		// ------------------------------------------------

		Vec3 row(uint num) const
		{
			return Vec3(mat[num][0], mat[num][1], mat[num][2]);
		}

		Vec3 col(uint num) const
		{
			return Vec3(mat[0][num], mat[1][num], mat[2][num]);
		}
	};

	/*
	 inline std::ostream& operator << (std::ostream& os, const Mat3T& mat){
	 for (int j = 0; j < 3; ++j)	{
	 for (int i = 0; i < 3; ++i)		{
	 os << mat[i][j] << ' ';
	 }

	 os << std::endl;
	 }

	 return os;
	 }
	 */

	template<typename T>
	inline Mat3T<T>::Mat3T(const T* d)
	{
		int index = 0;
		for (int j = 0; j < 3; ++j)
			for (int i = 0; i < 3; ++i)
				mat[i][j] = d[index++];
	}

	template<typename T>
	inline Mat3T<T>::Mat3T(T m00, T m01, T m02,
							T m10, T m11, T m12,
							T m20, T m21, T m22)
	{
		mat[0][0] = m00;	mat[0][1] = m01;	mat[0][2] = m02;
		mat[1][0] = m10;	mat[1][1] = m11;	mat[1][2] = m12;
		mat[2][0] = m20;	mat[2][1] = m21;	mat[2][2] = m22;
	}

	template<typename T>
	inline Mat3T<T> transposed(const Mat3T<T>& arg)
	{
		auto& m = arg.mat;

		return Mat3T<T>{
			m[0][0],  m[1][0],  m[2][0],
			m[0][1],  m[1][1],  m[2][1],
			m[0][2],  m[1][2],  m[2][2]
		};
	}

	template<typename T>
	inline T determinant(const Mat3T<T>& m)
	{
		return
			+ m.M(0,0) * m.M(1,1) * m.M(2,2)
			- m.M(0,0) * m.M(1,2) * m.M(2,1)
			+ m.M(0,1) * m.M(1,2) * m.M(2,0)
			- m.M(0,1) * m.M(1,0) * m.M(2,2)
			+ m.M(0,2) * m.M(1,0) * m.M(2,1)
			- m.M(0,2) * m.M(1,1) * m.M(2,0);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::inverse() const
	{
		Mat3T Adj;
		Adj[0][0] =  (mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1]);
		Adj[1][0] = -(mat[1][0]*mat[2][2] - mat[1][2]*mat[2][0]);
		Adj[2][0] =  (mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0]);
		Adj[0][1] = -(mat[0][1]*mat[2][2] - mat[0][2]*mat[2][1]);
		Adj[1][1] =  (mat[0][0]*mat[2][2] - mat[0][2]*mat[2][0]);
		Adj[2][1] = -(mat[0][0]*mat[2][1] - mat[0][1]*mat[2][0]);
		Adj[0][2] =  (mat[0][1]*mat[1][2] - mat[0][2]*mat[1][1]);
		Adj[1][2] = -(mat[0][0]*mat[1][2] - mat[0][2]*mat[1][0]);
		Adj[2][2] =  (mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0]);

		T det = mat[0][0]*Adj[0][0] + mat[0][1]*Adj[1][0] + mat[0][2]*Adj[2][0];

		if (fabs(det) < 0.000001f)
			return Mat3T();

		return Adj * (1.0f/det);
	}

	template<typename T>
	inline Mat3T<T> inverted(const Mat3T<T>& m)
	{
		return m.inverse();
	}

	template<typename T>
	inline T* Mat3T<T>::operator [] (int row)
	{
		return &mat[row][0];
	}

	template<typename T>
	inline const T* Mat3T<T>::operator [] (int row) const
	{
		return &mat[row][0];
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::operator *  (const Mat3T& rhs) const
	{
		const Mat3T& B = *this;
		const Mat3T& A = rhs;
		Mat3T M = Mat3T<T>::zero();

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				for (int k = 0; k < 3; ++k)
					M[i][j] += A[i][k] * B[k][j];

		return M;
	}

	template<typename T>
	inline const Mat3T<T>& Mat3T<T>::operator *= (const Mat3T& rhs)
	{
		*this = *this * rhs;

		return *this;
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::operator * (T s)
	{
		Mat3T M;
		for (int i=0; i < 3; ++i)
			for (int j=0; j < 3; ++j)
				M[i][j] = mat[i][j] * s;
		return M;
	}

	template<typename T>
	inline const Mat3T<T>& Mat3T<T>::operator *= (T s)
	{
		for (int i=0; i < 3; ++i)
			for (int j=0; j < 3; ++j)
				mat[i][j] *= s;
		return *this;
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::operator +  (const Mat3T& rhs) const
	{
		Mat3T result(*this);

		for (int j = 0; j < 3; ++j)
			for (int i = 0; i < 3; ++i)
				result[i][j] += rhs[i][j];

		return result;
	}

	template<typename T>
	inline const Mat3T<T>& Mat3T<T>::operator += (const Mat3T& rhs)
	{
		for (int j = 0; j < 3; ++j)
			for (int i = 0; i < 3; ++i)
				mat[i][j] += rhs[i][j];

		return *this;
	}

	template<typename T>
	inline bool operator == (const Mat3T<T>& a, const Mat3T<T>& b)
	{
		return
		a.mat[0][0] == b.mat[0][0] &&
		a.mat[0][1] == b.mat[0][1] &&
		a.mat[0][2] == b.mat[0][2] &&

		a.mat[1][0] == b.mat[1][0] &&
		a.mat[1][1] == b.mat[1][1] &&
		a.mat[1][2] == b.mat[1][2] &&

		a.mat[2][0] == b.mat[2][0] &&
		a.mat[2][1] == b.mat[2][1] &&
		a.mat[2][2] == b.mat[2][2];
	}

	// ------------------------------------------------

	template<typename T>
	inline Mat3T<T> Mat3T<T>::translate(T x, T y)
	{
		return Mat3T(1,0,0,
		             0,1,0,
		             x,y,1);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::rotate_x(T rad)
	{
		T c = cosf(rad);
		T s = sinf(rad);
		return Mat3T(1, 0, 0,
		             0, c,+s,
		             0,-s, c);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::rotate_y(T rad)
	{
		T c = cosf(rad);
		T s = sinf(rad);
		return Mat3T(+c,  0, -s,
		             0,   1,  0,
		             +s,  0, +c);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::rotate_z(T rad)
	{
		T c = cosf(rad);
		T s = sinf(rad);
		return Mat3T(+c, +s,  0,
		             -s, +c,  0,
		             0,   0,  1);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::scale(T x, T y)
	{
		return Mat3T(x,0,0,
						0,y,0,
						0,0,1);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::zero()
	{
		return Mat3T(0,0,0,
						0,0,0,
						0,0,0);
	}

	template<typename T>
	inline Mat3T<T> Mat3T<T>::identity()
	{
		return Mat3T(1,0,0,
						0,1,0,
						0,0,1);
	}

	// ------------------------------------------------

	template<typename T>
	inline Vec3T<T> mul(const Mat3T<T>& m, const Vec3T<T>& p)
	{
		return {
			m.mat[0][0]*p[0] + m.mat[1][0]*p[1] + m.mat[2][0]*p[2],
			m.mat[0][1]*p[0] + m.mat[1][1]*p[1] + m.mat[2][1]*p[2],
			m.mat[0][2]*p[0] + m.mat[1][2]*p[1] + m.mat[2][2]*p[2]
		};
	}

	template<typename T>
	inline Vec2T<T> mul_pos(const Mat3T<T>& m, const Vec2T<T>& p)
	{
		Vec3 v3 = mul(m, Vec3(p,1));
		return v3.xy / v3.z;
	}

	// Applies no translation. Use this for normals etc (though a renormalization is needed afterwards)
	// WARNING: m must have uniform scaling.
	template<typename T>
	inline Vec2T<T> mul_dir(const Mat3T<T>& m, const Vec2T<T>& d)
	{
		/*
		 We need to multiply with the transpose of the inverse of the upper-left 2x2 matrix:
		 | A   B|
		 | C   D|

		 Inversed is:
		 | D  -B|
		 |-C   A| / (A*D-B*C)

		 And that transposed is:
		 | D  -C|
		 |-B   A| / (A*D - B*C)

		 So we return:
		 (D*x - C*y, -B*x + A*y) / (A*D-B*C)
		 */
#if 0
		// Lazy
		assert( is_zero(dist(m.col(2), Vec3(0,0,1))) );
		assert( equals(m.row(0).length(xy), m.row(1).length(xy)) );
		assert( equals(m.col(0).length(xy), m.col(1).length(xy)) );

		return Vec2(m.mat[0][0]*d[0] + m.mat[1][0]*d[1],
						m.mat[0][1]*d[0] + m.mat[1][1]*d[1]);
#else
		// TODO: test :)
		return Vec2T<T>(
								 m.M(1,1)*d.x - m.M(0,1)*d.y,
								 m.M(0,0)*d.y - m.M(1,0)*d.x)
		/
		(m.M(0,0)*m.M(1,1) - m.M(0,1)*m.M(1,0));
#endif
	}

	// ------------------------------------------------

	using Mat3  = Mat3T<real>;
	using Mat3f = Mat3T<float>;
	using Mat3d = Mat3T<double>;
}

// ------------------------------------------------

namespace std {

template<typename T>
inline bool isfinite(const emath::Mat3T<T>& m)
{
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < 3; ++i) {
			if (!std::isfinite(m.mat[i][j])) {
				return false;
			}
		}
	}
	return true;
}

} // namespace std
