#include <windows.h>
#include <windowsx.h>
#include "Math.h"
#include "Rendering.h"

//Get a console handle
HWND g_window = GetConsoleWindow();
//Get a handle to device context
HDC g_hdc = GetDC(g_window);


void DrawTriangle(Vector2 p0, Vector2 p1, Vector2 p2, Color color)
{
	HPEN hPen = CreatePen(PS_SOLID, 0, RGB(color.r, color.g, color.b));
	HPEN hOldPen = SelectPen(g_hdc, hPen);

	HBRUSH hBrush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	HBRUSH hOldBrush = SelectBrush(g_hdc, hBrush);

	POINT vertices[3] = {
		{ (LONG)(p0.x + 0.5f), (LONG)(p0.y + 0.5f) },
		{ (LONG)(p1.x + 0.5f), (LONG)(p1.y + 0.5f) },
		{ (LONG)(p2.x + 0.5f), (LONG)(p2.y + 0.5f) }
	};
	Polygon(g_hdc, vertices, 3);

	SelectBrush(g_hdc, hOldBrush);
	DeleteObject(hBrush);

	SelectPen(g_hdc, hOldPen);
	DeleteObject(hPen);
}
void DrawTriangleFan(Vector2* points, int ptcount, Color color)
{
	POINT* vertices = new POINT[ptcount];
	for (int i = 0; i < ptcount; ++i)
	{
		vertices[i].x = (LONG)(points[i].x + 0.5f);
		vertices[i].y = (LONG)(points[i].y + 0.5f);
	}

	HPEN hPen = CreatePen(PS_SOLID, 0, RGB(color.r, color.g, color.b));
	HPEN hOldPen = SelectPen(g_hdc, hPen);

	HBRUSH hBrush = CreateSolidBrush(RGB(color.r, color.g, color.b));
	HBRUSH hOldBrush = SelectBrush(g_hdc, hBrush);

	Polygon(g_hdc, vertices, ptcount);
	delete[] vertices;

	SelectBrush(g_hdc, hOldBrush);
	DeleteObject(hBrush);

	SelectPen(g_hdc, hOldPen);
	DeleteObject(hPen);
}