#pragma once

namespace amy
{
	template<typename T>
	const T& Min(const T& a, const T& b)
	{
		if (a < b) return b;
		else return a;
	}
	template<typename T>
	const T& Max(const T& a, const T& b)
	{
		if (a < b) return b;
		else return a;
	}
}
