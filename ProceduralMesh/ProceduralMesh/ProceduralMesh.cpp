#include <Windows.h>
#include <chrono>
#include <thread>

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

// 3D coordinate in abstract space
struct Vec3
{
	double x, y, z;
};

struct Rot : Vec3
{

};

struct Color
{
	unsigned char r, g, b;
};

struct Transform
{
	Vec3 position, rotation, scale;
};

PixelPos Screenspace(Vec3 v)
{
	return { int(v.x + 0.5), int(v.y + 0.5) };
}

struct Poly
{
	Poly(size_t _sides, unsigned int* _v, Color _color) : m_sides(_sides), v(_v), color(_color) {};

	size_t m_sides;
	Color color; // The color value
	unsigned int* v; // v is an index in an array of verts

	const size_t GetSides()
	{
		return m_sides;
	}
};

// A polygon with 3 verticies
struct Tri : public Poly
{
	Tri() : Poly(3, v_mem, { 255,255,255 }) {};
	unsigned int v_mem[3];
};

// A polygon with 4 verticies
struct Quad : public Poly
{
	Quad() : Poly(4, v_mem, { 255,255,255 }) {};
	unsigned int v_mem[4];
};

// A polygon with n verticies
// Syntax is "NGon<n> ngonName"
template<size_t n>
struct NGon : public Poly
{
	NGon() : Poly(n, v_mem, { 255,255,255 }) {};
	unsigned int v_mem[n];
};

struct Mesh
{
	Vec3 m_pos;
	Vec3 m_rot;
	Vec3 m_scale;
	Vec3 m_vert[8];
	Poly m_poly[6];

	const int GetPoly(int request, Poly& output) 
	{
		Poly polygon = m_poly[request];
		output.v = polygon.v;
		return polygon.GetSides(); // Returns number of sides
	}

	bool InPoly()
	{

	}
};

void RenderMesh(Mesh mesh)
{

}

int main()
{

	return 0;
}