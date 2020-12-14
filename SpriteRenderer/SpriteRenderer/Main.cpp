#include <iostream>
#include "Rendering.h"

int main()
{
	g_frameBuffer.Draw();

	ReleaseDC(window, context);
	cin.ignore();

	return 0;
}