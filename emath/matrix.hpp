#pragma once

#include <vector>

#include <loguru.hpp>

namespace emath {

template<typename T>
class Matrix
{
public:
	Matrix() : _width(0), _height(0) {}
	Matrix(int width, int height)
		: _width(width), _height(height), _data(width * height) {}
	Matrix(int width, int height, T value)
		: _width(width), _height(height), _data(width * height, value) {}

	// ------------------------------------------------

	bool empty() const { return _data.empty(); }
	int width()  const { return _width;  } ///< The number of columns
	int height() const { return _height; } ///< The number of rows
	int total()  const { return _width * _height; }

	T*       data()       { return _data.data(); }
	const T* data() const { return _data.data(); }

	// ------------------------------------------------

	const T* begin() const { return data();           }
	T*       begin()       { return data();           }
	const T* end()   const { return data() + total(); }
	T*       end()         { return data() + total(); }

	// ------------------------------------------------

	bool contains_coord(int x, int y) const {
		return 0 <= x && x < _width && 0 <= y && y < _height;
	}

	T* row_ptr(int y)
	{
		DCHECK_F(0 <= y && y < _height, "%d not in range [0, %d)", y, _height);
		return data() + _width * y;
	}

	const T* row_ptr(int y) const
	{
		DCHECK_F(0 <= y && y < _height, "%d not in range [0, %d)", y, _height);
		return data() + _width * y;
	}

	T* pointer_to(int x, int y)
	{
		DCHECK_F(0 <= x && x < _width, "%d not in range [0, %d)", x, _width);
		DCHECK_F(0 <= y && y < _height, "%d not in range [0, %d)", y, _height);
		return data() + _width * y +   x;
	}

	/// (col, row)
	T& operator()(int x, int y) { return *pointer_to(x,y); }

	const T& operator()(int x, int y) const
	{
		DCHECK_F(0 <= x && x < _width, "%d not in range [0, %d)", x, _width);
		DCHECK_F(0 <= y && y < _height, "%d not in range [0, %d)", y, _height);
		return _data[_width * y + x];
	}

	template<typename Vec2Type>
	T& operator()(const Vec2Type& v) { return operator()(v.x, v.y); }

	template<typename Vec2Type>
	const T& operator()(const Vec2Type& v) const { return operator()(v.x, v.y); }

	// ------------------------------------------------

	std::vector<float> column(int x) const
	{
		DCHECK_F(0 <= x && x < _width, "%d not in range [0, %d)", x, _width);
		std::vector<float> column;
		column.reserve(_height);
		for (int y = 0; y < _height; ++y) {
			column.push_back(_data[_width * y +   x]);
		}
		return column;
	}

	// ------------------------------------------------

	// Keeps old values at the same place.
	void resize(int new_w, int new_h, const T& fill = T{})
	{
		Matrix new_matrix(new_w, new_h);

		for (int y = 0; y < new_h; ++y) {
			for (int x = 0; x < new_w; ++x) {
				if (x < _width && y < _height) {
					new_matrix(x, y) = (*this)(x, y);
				}
			}
		}
		*this = std::move(new_matrix);
	}

private:
	std::vector<T> _data;
	int            _width;
	int            _height;
};

// ----------------------------------------------------------------------------

template<typename T>
Matrix<T> operator*(const Matrix<T>& m, const T factor)
{
	Matrix<T> result(m.width(), m.height());
	for (int y = 0; y < m.height(); ++y) {
		for (int x = 0; x < m.width(); ++x) {
			result(x, y) = m(x, y) * factor;
		}
	}
	return result;
}

template<typename T>
Matrix<T> operator/(const Matrix<T>& m, const T factor)
{
	return m * (1 / factor);
}

// ----------------------------------------------------------------------------

using Matrixi = Matrix<int>;
using Matrixf = Matrix<float>;
using Matrixd = Matrix<double>;

} // namespace emath
