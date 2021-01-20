#pragma once
#include <vector>
#include <raylib.h>

extern const int g_windowWidth;
extern const int g_windowHeight;
extern const float g_gridSize;
extern Vector2 cursorPos;

template<typename Type>
size_t FindIndex(const std::vector<Type>& arr, Type elem)
{
	if (!arr.empty()) for (size_t i = 0; i < arr.size(); ++i) { if (arr[i] == elem) return i; }
	return arr.size();
}

template<typename Type>
void Erase(std::vector<Type>& arr, Type elem)
{
	size_t elementIndex = FindIndex(arr, elem);
	if (elementIndex != arr.size()) arr.erase(arr.begin() + elementIndex);
}
