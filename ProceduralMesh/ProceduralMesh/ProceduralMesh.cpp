#include <Windows.h>
#include <chrono>
#include <thread>
#include "Rendering.h"
#include "Mesh.h"

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device context
HDC hdc = GetDC(window);

void Sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 2D coordinate on the screen
struct PixelPos
{
	int x, y;
};

void DrawPixel(PixelPos p, unsigned char v)
{
	SetPixelV(hdc, p.x, p.y, RGB(v, v, v));
}

PixelPos Screenspace(Vector v)
{
	return { int(v.GetX() + 0.5), int(v.GetY() + 0.5) };
}

void RenderMesh(Mesh mesh)
{

}

int main()
{

	return 0;
}