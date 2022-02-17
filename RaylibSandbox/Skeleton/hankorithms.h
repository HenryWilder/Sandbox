#pragma once

namespace hw
{
#ifdef _VECTOR_

#ifdef _ALGORITHM_

	#define VECTOR_FIND(v,x) (std::find((v).begin(), (v).end(), (x)))
	#define VECTOR_CONTAINS(v,x) (VECTOR_FIND((v),(x)) != (v).end())

#else // !_ALGORITHM_

	namespace halg
	{
		template<class _InIt, class _Ty>
		_InIt find(const _InIt _First, const _InIt _Last, const _Ty& _Val)
		{
			for (_InIt iter = _First; iter != _Last; ++iter)
			{
				if (*iter == _Val)
					return iter;
			}
			return _Last;
		}
	}

	#define VECTOR_FIND(v,x) (halg::find((v).begin(), (v).end(), (x)))
	#define VECTOR_CONTAINS(v,x) (VECTOR_FIND((v),(x)) != (v).end())

#endif // _ALGORITHM_

	// Unique push
	template<class _Ty>
	void push_unique(std::vector<_Ty>& dest, const _Ty& src)
	{
		if (!VECTOR_CONTAINS(dest, src))
			dest.push_back(src);
	}

	// Push all
	template<typename _Ty>
	void multi_push_a(std::vector<_Ty>& dest, const std::vector<_Ty>& src)
	{
		dest.reserve(dest.size() + src.size());

		for (const _Ty& element : src)
		{
			dest.push_back(element);
		}
	}


	// Push unique
	template<typename _Ty>
	void multi_push(std::vector<_Ty>& dest, const std::vector<_Ty>& src)
	{
		dest.reserve(dest.size() + src.size());

		for (const _Ty& element : src)
		{
			if (!VECTOR_CONTAINS(dest, element))
				dest.push_back(element);
		}
	}

	// Erase matching
	template<typename _Ty>
	void multi_erase(std::vector<_Ty>& vec, const std::vector<_Ty>& removeVec)
	{
		size_t popCount = 0;
		auto pred = [&removeVec](const _Ty& element)
		{
			if (VECTOR_CONTAINS(removeVec, element))
			{
			popCount++;
			return false;
			}
			else
				return true;
		};
		std::stable_partition(vec.begin(), vec.end(), pred);
		while (popCount)
		{
			vec.pop_back();
			--popCount;
		}
	}

#ifdef _UNORDERED_SET_

	#define USET_FIND(s,x) (s).find((x))
	#define USET_CONTAINS(s,x) USET_FIND((s),(x)) != (s).end()

	//// Erase matching
	//template<typename _Ty>
	//void multi_erase(std::vector<_Ty>& vec, const std::unordered_set<_Ty>& remove)
	//{
	//	size_t popCount = 0;
	//	auto pred = [&remove](const _Ty& element)
	//	{
	//		bool pop = remove.find(element) != remove.end();
	//		popCount += (size_t)pop;
	//		return !pop;
	//	};
	//	std::count_if();
	//	std::stable_partition(vec.begin(), vec.end(), pred);
	//	while (popCount)
	//	{
	//		vec.pop_back();
	//		--popCount;
	//	}
	//}

#endif // _UNORDERED_SET_
#endif // _VECTOR_
}