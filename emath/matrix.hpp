#pragma once

#include <vector>

#include <loguru.hpp>

namespace emath {

/// May only contain POD types.
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

	int width()  const { return _width;  } ///< The number of columns
	int height() const { return _height; } ///< The number of rows
	int total()  const { return _width * _height; }

	T*       data()       { return _data.data(); }
	const T* data() const { return _data.data(); }

	// ------------------------------------------------

	T* row_ptr(int y)
	{
		CHECK_LT_F(y, _height);
		return data() + _width * y;
	}

	const T* row_ptr(int y) const
	{
		CHECK_LT_F(y, _height);
		return data() + _width * y;
	}

	T* pointer_to(int x, int y) const
	{
		CHECK_F(x < _width && y < _height);
		return data() + _width*y +   x;
	}

	/// (col, row)
	T& operator()(int x, int y) const { return *pointer_to(x,y); }

private:
	std::vector<T> _data;
	int            _width;
	int            _height;
};

} // namespace emath
