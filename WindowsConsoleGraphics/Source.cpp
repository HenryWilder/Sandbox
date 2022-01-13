#include <Windows.h>
#include <windowsx.h>
#include <fstream>

int main()
{
	std::ifstream pawn("pawn");

	HWND window = GetConsoleWindow();
	HDC hdc = GetWindowDC(window);

	DWORD cpt;
	pawn >> cpt;
	cpt *= 3;
	int r, g, b;
	pawn >> r >> g >> b;
	COLORREF col = RGB(r,g,b);

	POINT* apt = new POINT[cpt];
	for (size_t i = 0; i < 3; ++i)
	{
		pawn >> apt[i].x >> apt[i].y;
	}

	HPEN pen = CreatePen(0,0, col);
	HPEN oldPen = SelectPen(hdc, pen);
	HBRUSH brush = CreateSolidBrush(col);
	HBRUSH oldBrush = SelectBrush(hdc, brush);

	Polygon(hdc, apt, cpt);

	SelectBrush(hdc, oldBrush);
	DeleteBrush(brush);
	SelectPen(hdc, oldPen);
	DeletePen(brush);

	delete[] apt;
	pawn.close();

	return 0;
}