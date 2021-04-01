#pragma once

namespace amy
{
	template<typename T, int dimensions>
	class position
	{
	private:
		T elements[dimensions];

	public:
		template<int x>
		T& get()
		{
			return elements[x];
		}

		int dim()
		{
			return dimensions;
		}

		T length()
		{
			T sum = 0;
			for (T& elem : elements)
			{
				sum += elem;
			}
			return (T)sqrt(sum);
		}

		position normal()
		{
			position normal;
			T len = length();
			for (int e = 0; e < dimensions; ++e)
			{
				normal[e] = elements[e] / len;
			}
			return normal;
		}

		static position operator+(position pt1, position pt2)
		{
			position sum;
			for (int e = 0; e < dimensions; ++e)
			{
				sum[e] = pt1.elements[e] + pt2.elements[e];
			}
			return sum;
		}
		static position operator-(position pt1, position pt2)
		{
			position sum;
			for (int e = 0; e < dimensions; ++e)
			{
				sum[e] = pt1.elements[e] - pt2.elements[e];
			}
			return sum;
		}
		static position operator*(position pt1, position pt2)
		{
			position sum;
			for (int e = 0; e < dimensions; ++e)
			{
				sum[e] = pt1.elements[e] * pt2.elements[e];
			}
			return sum;
		}
		static position operator/(position pt1, position pt2)
		{
			position sum;
			for (int e = 0; e < dimensions; ++e)
			{
				sum[e] = pt1.elements[e] / pt2.elements[e];
			}
			return sum;
		}

		static T distance(position pt1, position pt2)
		{
			T sum = 0;
			for (int e = 0; e < dimensions; ++e)
			{
				sum += (pt1.elements[e] - pt2.elements[e]);
			}
			return (T)sqrt(sum);
		}
	};
}