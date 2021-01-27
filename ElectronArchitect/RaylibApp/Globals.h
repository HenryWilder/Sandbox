#pragma once
#include <vector>

struct Vector2;
struct Int2;

extern const int g_windowWidth;
extern const int g_windowHeight;

Vector2 Vector2InvertY(Vector2 vector);

extern const int g_gridSize; // Width of a gridspace in pixels at 1x zoom
extern Int2 cursorPos;

template<typename Type>
size_t FindIndex(const std::vector<Type>& arr, const Type elem)
{
	if (!arr.empty())
	{
		for (size_t i = 0; i < arr.size(); ++i)
		{
			if (arr[i] == elem) return i;
		}
	}
	return arr.size();
}

template<typename Type>
void Erase(std::vector<Type>& arr, const Type elem)
{
	size_t elementIndex = FindIndex(arr, elem);
	if (elementIndex != arr.size())
	{
		arr.erase(arr.begin() + elementIndex);
	}
}

template<typename Type>
void EraseMulti(std::vector<Type>& arr, const std::vector<Type>& selection)
{
	for (Type elem : selection)
	{
		Erase<Type>(arr, elem);
	}
}

template<typename Type>
size_t FindInMemBlock(const Type* const block, const size_t blockSize, const Type elem)
{
	if (block)
	{
		for (size_t i = 0; i < blockSize; ++i)
		{
			if (block[i] == elem) return i;
		}
	}
	return blockSize;
}

template<typename Type>
void Nullify(Type* const block, const size_t blockSize, const Type elem)
{
	size_t elementIndex = FindInMemBlock<Type>(block, blockSize, elem);
	if (elementIndex != blockSize)
	{
		if (std::is_pointer<Type>::value)
		{
			block[elementIndex] = nullptr;
		}
		else
		{
			memset(block[elementIndex], 0, sizeof(Type));
		}
	}
}
