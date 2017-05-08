#include <functional>

#include "fwd.hpp"
#include "int.hpp" // is_power_of_two, HUGE_PRIME_0

namespace std {

template<typename T>
struct hash<emath::Vec2T<T>>
{
	size_t operator()(const emath::Vec2T<T>& val) const
	{
		return std::hash<T>()(val.x) + std::hash<T>()(val.y) * emath::HUGE_PRIME_0;
	}
};

template<typename T>
struct hash<emath::Vec3T<T>>
{
	size_t operator()(const emath::Vec3T<T>& val) const
	{
		return std::hash<T>()(val.x) + std::hash<T>()(val.y) * emath::HUGE_PRIME_0 + std::hash<T>()(val.z) * emath::HUGE_PRIME_1;
	}
};

} // namespace std
