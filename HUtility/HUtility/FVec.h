#pragma once
#include <type_traits>
#include <concepts>
#include <algorithm>

// Continuous Math Vector Concepts
namespace cmvc
{
	// Position style vector
	template<typename Vector_t, unsigned dimensions>
	concept ContinuousMathVector_Position = requires(Vector_t vec) {
		std::floating_point<decltype(vec.x)> &&
			(sizeof(vec) == sizeof(vec.x) * dimensions);
	};

	// Color style vector
	template<typename Vector_t, unsigned dimensions>
	concept ContinuousMathVector_Color = requires(Vector_t vec) {
		std::floating_point<decltype(vec.r)> &&
			(sizeof(vec) == sizeof(vec.r) * dimensions);
	};

	// Array style vector
	template<typename Vector_t, unsigned dimensions>
	concept ContinuousMathVector_Array = requires(Vector_t vec) {
		std::floating_point<decltype(vec[0])> &&
			(sizeof(vec) == sizeof(vec[0]) * dimensions);
	};

	// Data-array style vector
	template<typename Vector_t, unsigned dimensions>
	concept ContinuousMathVector_DataArray = requires(Vector_t vec) {
		std::floating_point<decltype(vec.data[0])> &&
			(sizeof(vec.data) == sizeof(vec.data[0]) * dimensions);
	};
}

template<std::floating_point T, unsigned dimensions>
struct FVec
{
	template<ContinuousMathVector_Position<dimensions> SrcVector_t> FVec(SrcVector_t vec) { vec }

};
using Vec2 = FVec<float, 2>;
using Vec3 = FVec<float, 3>;
using Vec4 = FVec<float, 4>;

using Vec2D = FVec<double, 2>;
using Vec3D = FVec<double, 3>;
using Vec4D = FVec<double, 4>;
