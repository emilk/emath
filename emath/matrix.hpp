#pragma once

#include "complex.hpp"
#include "range.hpp"
#include "fwd.hpp"
#include "vector.hpp"

namespace emath
{
	/* Example of how to create an 100x100 identity matrix:
	  Matrix m = Matrix::allocate(100, 100, 0.0f);
	  m.get_diag().fill(1);
	*/
	template<typename T>
	class ConstMatrix;

	template<typename T>
	class Matrix {
	public:
		forceinline static Matrix allocate(int w, int h) {
			//int stride = Data::alignT<T>(h);
			int stride = w;
			DataPtr data = DataPtr::allocate(stride*h*sizeof(T));
			return Matrix( data,
						reinterpret_cast<T*>(data.ptr()),
						stride, w, h );
		}

		// Allocate and fill
		forceinline static Matrix allocate(int w, int h, const T& val) {
			Matrix ret = allocate(w,h);
			ret.fill(val);
			return ret;
		}

		forceinline static Matrix allocate(Vec2i size) {
			return allocate(size.width, size.height);
		}

		forceinline static Matrix allocate(Vec2i size, const T& val) {
			return allocate(size.width, size.height, val);
		}

		// ------------------------------------------------

		Matrix() : _ptr(nullptr), _stride(0), _width(0), _height(0) {}

		// Advanced usage!
		Matrix(const DataPtr& data, T* ptr, unsigned stride, unsigned w, unsigned h)
			: _data(data), _ptr(ptr), _stride(stride), _width(w), _height(h)
		{ }

		// ------------------------------------------------

		forceinline T* row_ptr(int y) const
		{
			CHECK_F(_ptr && y<_height);
			return _ptr + _stride*y;
		}

		forceinline T* pointer_to(int x, int y) const
		{
			CHECK_F(_ptr && x<_width && y<_height);
			return _ptr + _stride*y + x;
		}

		// (col,row)
		forceinline T& operator()(int x, int y) const { return *pointer_to(x,y); }

		// Return a reference to a row.
		forceinline const Vector<T> slice1D(int x, int y, int w) const
		{
			if (w==0) return Vector<T>();
			CHECK_F(_ptr && x+w<=_width && y<_height);
			return Vector<T>(_data, pointer_to(x,y), w);
		}

		// Return a reference to a rectangular area.
		forceinline const Matrix<T> slice2D(int x, int y, int w, int h) const
		{
			if (w*h==0) return Matrix<T>();
			CHECK_F(_ptr && x+w<=_width && y+h<=_height);
			return Matrix<T>(_data, pointer_to(x,y), _stride, w, h); // Same stride
		}

//		forceinline const Matrix<T> slice2D(const QRect& rect) const
{
//			return slice2D(rect.left(), rect.top(), rect.width(), rect.height());
//		}

		forceinline const Matrix<T> slice2D(const RangeI& xRange, const RangeI& yRange) const
		{
			return slice2D(xRange.begin(), yRange.begin(), xRange.size(), yRange.size());
		}

		// Return a reference to the diagonal elements
		forceinline const Matrix<T> diagD() const
		{
			int s = min(width(), height());
			return Matrix<T>(_data, _ptr, _stride+1, s, s);
		}

		// ------------------------------------------------

		forceinline bool is_nullptr() const { return _data.is_nullptr(); }
		forceinline int width() const  { return _width; } // The number of columns
		forceinline int height() const { return _height; } // The number of rows
		forceinline Vec2i size() const { return Vec2i(width(), height()); }
		forceinline const DataPtr& data() const { return _data; }

		// WARNING! The matrix may NOT be a slice!
		const T* start_ptr() const
		{
			if (is_nullptr()) return nullptr;
			CHECK_F(_stride==_width);
			return _ptr;
		}

		// ------------------------------------------------

		void fill(const T& v) const
		{
			for (int ri=0; ri<_height; ++ri)
				MemSet(row_ptr(ri), _width, v);
		}

		void assign(const ConstMatrix<T>& mat) const
		{
			CHECK_F(mat.size()==this->size());
			for (int ri=0; ri<_height; ++ri)
				MemCpy(this->row_ptr(ri), mat.row_ptr(ri), _width);
		}

		// Multiply each element in me with corresponding element in mat. Multiplying with zero always gives zero!
		template<typename T2>
		void comp_mul_with(const ConstMatrix<T2>& mat) const
		{
			CHECK_F(mat.size()==this->size());

			for (int ri=0; ri<_height; ++ri) {

				T* this_row = this->row_ptr(ri);
				const T2* rhs_row = mat.row_ptr(ri);

				for (int ci=0; ci<_width; ++ci) {
					if (this_row[ci]==T(0) || rhs_row[ci]==T2(0))
						this_row[ci] = T(0); // Fixes singularity issues (e.g.,  0/0*0 == 0)
					else
						this_row[ci] *= rhs_row[ci];
				}
			}
		}

		// Divide each element in me with corresponding element in mat
		template<typename T2>
		void comp_div_with(const ConstMatrix<T2>& mat) const
		{
			CHECK_F(mat.size()==this->size());

			for (int ri=0; ri<_height; ++ri) {

				T* this_row = this->row_ptr(ri);
				const T2* rhs_row = mat.row_ptr(ri);

				for (int ci=0; ci<_width; ++ci)
					this_row[ci] /= rhs_row[ci];
			}
		}

		template<typename T2>
		void comp_mul_with(const Matrix<T2>& v) const { return comp_mul_with(ConstMatrix<T2>(v)); }

	protected:
		friend class ConstMatrix<T>;

	private:
		DataPtr _data;  // For ref-counting only.
		T* _ptr;        // Start of first element.

		// Unit* is number of T:s. stride is the number of elements per row, >= _width.
		int _stride; // _ptr + N*_stride == start of row N.
		int _width;
		int _height;
	};

	// ------------------------------------------------
	// ------------------------------------------------
	// ------------------------------------------------

	template<typename T>
	class ConstMatrix {
	public:
		static forceinline ConstMatrix from_row_vec(const ConstVector<T>& vec) {
			return ConstMatrix(vec.data(), vec.ptr(), vec.size(), vec.size(), 1);
		}

		// ------------------------------------------------

		ConstMatrix() : _ptr(nullptr), _stride(0), _width(0), _height(0) {}
		ConstMatrix(const ConstMatrix& o) : _data(o._data), _ptr(o._ptr), _stride(o._stride), _width(o._width), _height(o._height) {}
		ConstMatrix(const Matrix<T>&   o) : _data(o._data), _ptr(o._ptr), _stride(o._stride), _width(o._width), _height(o._height) {}

		ConstMatrix(const ConstDataPtr& data, const T* ptr, int stride, int w, int h)
			: _data(data), _ptr(ptr), _stride(stride), _width(w), _height(h)
		{ }

		// ------------------------------------------------

		forceinline const T* row_ptr(int y) const { CHECK_F(_ptr && y<_height); return _ptr + _stride*y; }
		// m(col,row)
		forceinline const T* pointer_to(int x, int y) const { CHECK_F(_ptr && x<_width && y<_height); return _ptr + _stride*y + x; }
		forceinline const T& operator()(int x, int y) const { return *pointer_to(x,y); }
		forceinline const T& operator()(const Vec2i& p) const { return *pointer_to(p.x, p.y); }

		forceinline const ConstVector<T> row(int ix) const
		{
			return ConstVector<T>(_data, row_ptr(ix), width());
		}

		forceinline const ConstVector<T> slice1D(int x, int y, int w) const
		{
			if (w==0) return ConstVector<T>();
			CHECK_F(_ptr && x+w<=_width && y<_height);
			return ConstVector<T>(_data, pointer_to(x,y), w);
		}

		// Return a reference to a rectangular area.
		forceinline const ConstMatrix<T> slice2D(int x, int y, int w, int h) const
		{
			if (w*h==0) return ConstMatrix<T>();
			CHECK_F(x+w<=_width && y+h<=_height);
			return ConstMatrix<T>(_data, pointer_to(x,y), _stride, w, h); // Same stride
		}

//		forceinline const ConstMatrix<T> slice2D(const QRect& rect) const
{
//			return slice2D(rect.left(), rect.top(), rect.width(), rect.height());
//		}

		forceinline const ConstMatrix<T> slice2D(const RangeI& xRange, const RangeI& yRange) const
		{
			return slice2D(xRange.begin(), yRange.begin(), xRange.size(), yRange.size());
		}

		// Return a reference to the diagonal elements
		forceinline const ConstMatrix<T> diagD() const
		{
			int s = min(width(), height());
			return ConstMatrix<T>(_data, _ptr, _stride+1, s, s);
		}

		// Will copy the entire matrix!
		forceinline const Matrix<T> clone() const
		{
			Matrix<T> ret = Matrix<T>::allocate(size());
			ret.assign(*this);
			return ret;
		}

		// ------------------------------------------------

		forceinline bool is_nullptr() const { return _data.is_nullptr(); }
		forceinline int width() const  { return _width; } // The number of columns
		forceinline int height() const { return _height; } // The number of rows
		forceinline Vec2i size() const { return Vec2i(width(), height()); }
		forceinline const ConstDataPtr& data() const { return _data; }

		// WARNING! The matrix may NOT be a slice!
		const T* start_ptr() const
		{
			if (is_nullptr()) return nullptr;
			CHECK_F(_stride==_width);
			return _ptr;
		}

	private:
		ConstDataPtr _data;  // For ref-counting only.
		const T* _ptr;        // Start of first element.

		// Unit* is number of T:s. stride is the number of elements per row, >= _width.
		int _stride; // _ptr + N*_stride == start of row N.
		int _width;
		int _height;
	};

	// ------------------------------------------------

	using MatR          = Matrix<real>;
	using MatC          = Matrix<complex>;
	using ConstMatR     = ConstMatrix<real>;
	using ConstMatC     = ConstMatrix<complex>;
	using MatInt        = Matrix<int>;
	using ConstMatInt   = ConstMatrix<int>;
	using MatInt16      = Matrix<int16_t>;
	using ConstMatInt16 = ConstMatrix<int16_t>;

	// ------------------------------------------------
	// ------------------------------------------------
	// ------------------------------------------------
	// OPERATORS

	// ------------------------------------------------
	// Matrix element

	template<typename T, typename T2>
	const Matrix<T>& operator*=(const Matrix<T>& lhs, const T2& rhs) {
		for (int ri=0; ri<lhs.height(); ++ri) {
			T* l = lhs.row_ptr(ri);
			int n=lhs.width();
			for (int i=0; i<n; ++i)
				l[i] *= rhs;
		}
		return lhs;
	}

	// Per element
	template<typename T, typename T2>
	const Matrix<T> operator-(const T2& val, const ConstMatrix<T>& mat) {
		Matrix<T> ret = Matrix<T>::allocate(mat.size());
		for (int ri=0; ri<ret.height(); ++ri)
			for (int ci=0; ci<ret.width(); ++ci)
				ret(ci,ri) = val - mat(ci,ri);
		return ret;
	}

	template<typename T>
	const Matrix<T>& operator-=(const Matrix<T>& left, const ConstMatrix<T>& right) {
		CHECK_F(left.size()==right.size());
		for (int ri=0; ri<left.height(); ++ri)
			for (int ci=0; ci<left.width(); ++ci)
				left(ci,ri) -= right(ci,ri);
		return left;
	}

	template<typename T>
	const Matrix<T> operator-(const ConstMatrix<T>& left, const ConstMatrix<T>& right) {
		Matrix<T> ret = left.clone();
		ret -= right;
		return ret;
	}

	// ------------------------------------------------

	template<typename T>
	inline T max_elem(const ConstMatrix<T>& m) {
		T max=0;
		for (int y=0; y<m.height(); ++y)
			for (int x=0; x<m.width(); ++x)
				max = max(max, m(x,y));
		return max;
	}

	template<typename T>
	const Matrix<T> transposed(const ConstMatrix<T>& mat) {
		Matrix<T> ret = Matrix<T>::allocate(mat.height(), mat.width());
		for (int ri=0; ri<mat.height(); ++ri)
			for (int ci=0; ci<mat.width(); ++ci)
				ret(ri,ci) = mat(ci,ri);
		return ret;
	}

	template<typename T>
	const Matrix<T> transposed(const Matrix<T>& mat) {
		return transposed(ConstMatrix<T>(mat));
	}

	// ------------------------------------------------

	// Outer product. Returned matrix will be [w,h]=[rowV.size(), colV.size()]
	inline MatR outer(const ConstVecR& colV, const ConstVecR& rowV) {
		int w = rowV.size();
		int h = colV.size();
		MatR m = MatR::allocate(w,h);
		for (int yi=0; yi<h; ++yi)
			for (int xi=0; xi<w; ++xi)
				m(xi,yi) = colV(yi)*rowV(xi);
		return m;
	}

	// Ease-in, ease-out
	inline MatR make_soft_window(int w, int h) {
		ConstVecR x=make_soft_window(w);
		ConstVecR y=make_soft_window(h);
		return outer(y,x);
	}

	template<typename T>
	void soften_left_right(emath::Matrix<T>& m, bool left, bool right) {
		if (m.width() < 3)
			return;

		int soft_w = min<int>(4, m.width()/2*2);
		MatR s = outer(VecR::allocate(m.height(), 1.0f), make_soft_window(soft_w));
		CHECK_F(s.height()==m.height()); // Over full time range

		if (left)
			m.slice2D(0,                  0, soft_w/2, m.height()).comp_mul_with( s.slice2D(0,        0, soft_w/2, m.height()) ); // Left
		if (right)
			m.slice2D(m.width()-soft_w/2, 0, soft_w/2, m.height()).comp_mul_with( s.slice2D(soft_w/2, 0, soft_w/2, m.height()) ); // Right
	}

	template<typename T>
	void soften_top_bottom(emath::Matrix<T>& m, bool top, bool bottom) {
		if (m.height() < 3)
			return;

		int soft_h = min<int>(4, m.height()/2*2);
		MatR s = outer(make_soft_window(soft_h), VecR::allocate(m.width(), 1.0f));
		CHECK_F(s.width()==m.width()); // Over full frequency range
		if (top)
			m.slice2D(0,                   0, m.width(), soft_h/2).comp_mul_with( s.slice2D(0,        0, m.width(), soft_h/2) ); // Top
		if (bottom)
			m.slice2D(0, m.height()-soft_h/2, m.width(), soft_h/2).comp_mul_with( s.slice2D(0, soft_h/2, m.width(), soft_h/2) ); // Bottom
	}

}
