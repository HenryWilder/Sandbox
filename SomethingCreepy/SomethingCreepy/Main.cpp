#include <Windows.h>

// Get a console handle
HWND window = GetConsoleWindow();

// Get a handle to device context
HDC hdc = GetDC(window);

int main()
{
	HDC desktop = GetDC(NULL);

	for (int y = 0; y < 1080; ++y) {
		for (int x = 0; x < 1920; ++x) {
			SetPixelV(desktop, x, y, RGB(0, 0, 0));
		}
	}

	return 0;
}