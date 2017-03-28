#pragma once

#include "math.hpp"

namespace emath
{
	// [begin, end)
	class RangeI
	{
	public:
		RangeI() : _begin(-1), _end(-2) {}
		RangeI(int begin, int end) : _begin(begin), _end(end) {}

		int begin() const { return _begin;       }
		int end()   const { return _end;         }
		int size()  const { return _end-_begin; }

		void include(int val) {
			_begin = min(_begin, val  );
			_end   = max(_end,   val+1);
		}

		/* RangeI division works like this:
		 Assume we have chunks of size 'n', which chunks
		 are covered by 'range'?

		 This is rounded so as to be inclusive. Example:

		 The dots is our original integer range.
		 The x:es is the new range (3 to one, n=3).

		 [    )       // Original range: [5, 10)
		 ................
		 x  x  x  x  x  z
		 [        )     // New range: [1, 4)

		 */
		friend const RangeI operator/(const RangeI& range, int n) {
			return RangeI(
							  range.begin()/n,
							  (range.end()+n-1)/n);
		}

		// Reverse of above
		friend const RangeI operator*(const RangeI& range, int n) {
			return RangeI(
							  range.begin()*n,
							  range.end()*n);
		}

		// Shift
		friend const RangeI operator+(const RangeI& range, int n) {
			return RangeI(
							  range.begin()+n,
							  range.end()+n);
		}

		friend const RangeI operator-(const RangeI& range, int n) {
			return RangeI(
							  range.begin()-n,
							  range.end()-n);
		}

		// Clamps a range to not contain elements outside [0, size)
		// note that this can return the empty range [size,size)
		const RangeI clamped(int size) const
		{
			return RangeI(
							  emath::clamp<int>(begin(), 0, size),
							  emath::clamp<int>(end(),   0, size)
							  );
		}

	private:
		int _begin, _end;
	};

	class RangeI_iterator
	{
	public:
		RangeI_iterator(int val) : _val(val) { }

		int  operator*() const { return _val; }
		int& operator*()       { return _val; }

	private:
		int _val;
	};

	inline RangeI_iterator begin(RangeI r) { return r.begin(); }
	inline RangeI_iterator end  (RangeI r) { return r.end();   }

	// ------------------------------------------------

	// [begin, end). Normally used for time/frequency ranges. hence double precision.
	template<typename T>
	class RangeFloat
	{
	public:
		RangeFloat() : _min(+emath::INF<T>), _max(-emath::INF<T>) {}
		RangeFloat(T begin, T end) : _min(begin), _max(end) {}
		explicit RangeFloat(const RangeI& int_range) : _min(int_range.begin()), _max(int_range.end()) {}

		static const RangeFloat Empty()      { return RangeFloat();          }
		static const RangeFloat Everything() { return RangeFloat(-emath::INF<T>, +emath::INF<T>); }

		T begin()  const { return _min;             }
		T end()    const { return _max;             }
		T min()    const { return _min;             }
		T max()    const { return _max;             }
		T size()   const { return _max-_min;       }
		T center() const { return 0.5f*(_min+_max); }

		// positive size
		bool is_valid() const
		{
			return size()>0;
		}

		void include(T val) {
			_min = emath::min(_min, val);
			_max = emath::max(_max, val);
		}

		// Returns the smallest integer RangeI completely containing this RangeFloat
		RangeI to_aligned_range() const
		{
			return RangeI(floor_to_int(_min), ceil_to_int(_max));
		}

	private:
		T _min, _max;
	};

	using RangeF = RangeFloat<float>;
	using RangeD = RangeFloat<double>;
} // namespace emath


// namespace std {

// template<typename T>
// bool isfinite(const emath::RangeF<T>& range)
// {
// 	return std::isfinite(range.min()) && std::isfinite(range.max());
// }
// } // namespace std
