#pragma once

#include <vector>

#include <loguru.hpp>

#include "fwd.hpp"

namespace emath {

template<typename T>
class Matrix
{
public:
	typedef       T  value_type;
	typedef const T* const_iterator;
	typedef       T* iterator;

	Matrix() : _width(0), _height(0) {}

	Matrix(int width, int height)
		: _width(width), _height(height), _data(width * height) {}

	Matrix(int width, int height, T value)
		: _width(width), _height(height), _data(width * height, value) {}

	Matrix(int width, int height, std::vector<T> data)
		: _width(width), _height(height), _data(data)
	{
		CHECK_EQ_F(width * height, _data.size());
	}

	Matrix(int width, int height, const T* data)
		: _width(width), _height(height)
	{
		if (width * height > 0) {
			CHECK_NOTNULL_F(data);
			_data = std::vector<T>(data, data + width * height);
		}
	}

	Matrix(std::initializer_list<std::initializer_list<T>> rows)
	{
		_height = rows.size();
		if (rows.size() == 0) {
			_width = 0;
			return;
		}
		_width = rows.begin()->size();
		_data.reserve(_width * _height);
		for (const auto& row : rows) {
			CHECK_EQ_F(row.size(), _width);
			for (const auto& value : row) {
				_data.push_back(value);
			}
		}
	}

	// ------------------------------------------------

	bool empty() const { return _data.empty(); }
	int width()  const { return _width;  } ///< The number of columns
	int height() const { return _height; } ///< The number of rows
	int size()   const { return static_cast<int>(_data.size()); }

	T*       data()       { return _data.data(); }
	const T* data() const { return _data.data(); }

	// NOTE: you may NOT change the length of the returned vector, ONLY the contents!
	std::vector<T>& as_vec() { return _data; }
	const std::vector<T>& as_vec() const { return _data; }

	// ------------------------------------------------

	const T* begin()  const { return data();          }
	const T* cbegin() const { return data();          }
	T*       begin()        { return data();          }
	const T* end()    const { return data() + size(); }
	const T* cend()   const { return data() + size(); }
	T*       end()          { return data() + size(); }

	// ------------------------------------------------

	bool contains_coord(int x, int y) const
	{
		return 0 <= x && x < _width && 0 <= y && y < _height;
	}

	bool contains_coord(const Vec2i& c) const { return contains_coord(c.x, c.y); }

	// ------------------------------------------------

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

	// ------------------------------------------------

	T& operator[](const Vec2i& v) { return operator()(v.x, v.y); }
	const T& operator[](const Vec2i& v) const { return operator()(v.x, v.y); }

	T& operator[](const int flat) {
		DCHECK_GE_F(flat, 0);
		DCHECK_LT_F(flat, size());
		return _data[flat];
	}

	const T& operator[](const int flat) const {
		DCHECK_GE_F(flat, 0);
		DCHECK_LT_F(flat, size());
		return _data[flat];
	}

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

	// ------------------------------------------------

	template<typename X>
	Matrix<X> cast() const
	{
		auto m = Matrix<X>(_width, _height);
		for (int i = 0; i < _data.size(); ++i) {
			m[i] = static_cast<X>(_data[i]);
		}
		return m;
	}

	Matrix<T> transpose() const
	{
		auto m = Matrix<T>(_height, _width);
		for (int y = 0; y < _height; ++y) {
			for (int x = 0; x < _width; ++x) {
				m(y, x) = (*this)(x, y);
			}
		}
		return m;
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
	for (int i = 0; i < m.size(); ++i) {
		result[i] = m[i] * factor;
	}
	return result;
}

template<typename T>
Matrix<T> operator/(const Matrix<T>& m, const T factor)
{
	return m * (1 / factor);
}

template<typename T>
void operator*=(Matrix<T>& m, const T factor)
{
	for (auto& v : m) {
		v *= factor;
	}
}

template<typename T>
void operator/=(Matrix<T>& m, const T factor)
{
	m *= (1 / factor);
}

template<typename T>
Matrix<T> operator-(const Matrix<T>& m)
{
	Matrix<T> result(m.width(), m.height());
	for (int i = 0; i < m.size(); ++i) {
		result[i] = -m[i];
	}
	return result;
}

// ----------------------------------------------------------------------------

using Matrixi = Matrix<int>;
using Matrixf = Matrix<float>;
using Matrixd = Matrix<double>;

} // namespace emath
