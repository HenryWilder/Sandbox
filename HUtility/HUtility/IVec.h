#pragma once
#include <type_traits>
#include <concepts>
#include <algorithm>

// Discrete Math Vector Concepts
namespace dmvc
{
	// Position style vector
	template<typename Vector_t, unsigned dimensions>
	concept DiscreteMathVector_Position = requires(Vector_t vec) {
		std::integral<decltype(vec.x)> &&
			(sizeof(vec) == sizeof(vec.x) * dimensions);
	};

	// Color style vector
	template<typename Vector_t, unsigned dimensions>
	concept DiscreteMathVector_Color = requires(Vector_t vec) {
		std::integral<decltype(vec.r)> &&
			(sizeof(vec) == sizeof(vec.r) * dimensions);
	};

	// Array style vector
	template<typename Vector_t, unsigned dimensions>
	concept DiscreteMathVector_Array = requires(Vector_t vec) {
		std::integral<decltype(vec[0])> &&
			(sizeof(vec) == sizeof(vec[0]) * dimensions);
	};

	// Data-array style vector
	template<typename Vector_t, unsigned dimensions>
	concept DiscreteMathVector_DataArray = requires(Vector_t vec) {
		std::integral<decltype(vec.data[0])> &&
			(sizeof(vec.data) == sizeof(vec.data[0]) * dimensions);
	};

	// Data-array style vector
	template<typename Vector_t, unsigned dimensions>
	concept DiscreteMathVector =
		DiscreteMathVector_Position<Vector_t, dimensions> ||
		DiscreteMathVector_Color<Vector_t, dimensions> ||
		DiscreteMathVector_Array<Vector_t, dimensions> ||
		DiscreteMathVector_DataArray<Vector_t, dimensions>;
}

template<std::integral T, unsigned dimensions>
struct IVec
{
	template<typename T, dmvc::DiscreteMathVector<T, dimensions> SrcVector_t>
	constexpr IVec(const SrcVector_t& vec)
	{
		for (unsigned i = 0; i < sizeof(T) * dimensions; ++i)
			this[i] = (&vec)[i];
	}

	union
	{
		// Position
		struct requires(dimensions == 1) { T x; };
		struct requires(dimensions == 2) { T x, y; };
		struct requires(dimensions == 3) { T x, y, z; };
		struct requires(dimensions >= 4) { T x, y, z, w; };

		// Color
		struct requires(dimensions == 1) { T r; };
		struct requires(dimensions == 2) { T r, g; };
		struct requires(dimensions == 3) { T r, g, b; };
		struct requires(dimensions >= 4) { T r, g, b, a; };

		// Data array
		struct { T data[dimensions]; };
	};

	// Array
	T& operator[](unsigned index)
	{
		_ASSERTE(index < dimensions);
		return data[index];
	}

	// Dynamic const get
	const T& operator[](unsigned index) const
	{
		_ASSERTE(index < dimensions);
		return data[index];
	}

	// constexpr get
	constexpr T operator[](unsigned index) const
	{
		static_assert(index < dimensions);
		return data[index];
	}
};

using IVec2 = IVec<int, 2>;
using IVec3 = IVec<int, 3>;
using IVec4 = IVec<int, 4>;

using LVec2 = IVec<long, 2>;
using LVec3 = IVec<long, 3>;
using LVec4 = IVec<long, 4>;
