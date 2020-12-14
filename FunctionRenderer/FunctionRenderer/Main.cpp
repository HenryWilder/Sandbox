#include "Render.h"
#include <chrono>
#include <thread>

void Sleep(int x) {
	std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

int main()
{
	DrawPixel(40, 5, { 255,255,255 });
	return 0;
}