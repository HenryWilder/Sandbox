#include <Windows.h>

struct Vert
{
	float x, y, z;
};

struct Poly
{
	Poly(size_t _sides, unsigned int* _v) : m_sides(_sides), v(_v) {};

	size_t m_sides;
	unsigned int* v; // v is an index in an array of verts

	bool Contains(Vert);

	const size_t GetSides()
	{
		return m_sides;
	}
};

// A polygon with 3 verticies
struct Tri : public Poly
{
	Tri() : Poly(3, v_mem) {};
	unsigned int v_mem[3];
};

// A polygon with 4 verticies
struct Quad : public Poly
{
	Quad() : Poly(4, v_mem) {};
	unsigned int v_mem[4];
};

// A polygon with n verticies
// Syntax is "NGon<n> ngonName"
template<size_t n>
struct NGon : public Poly
{
	NGon() : Poly(n, v_mem) {};
	unsigned int v_mem[n];
};

struct Mesh
{
	Vert m_vert[8];
	Poly m_poly[6];

	const int GetPoly(int request, Poly& output) 
	{
		Poly polygon = m_poly[request];
		output.v = polygon.v;
		return polygon.GetSides(); // Returns number of sides
	}
};

int main()
{

	return 0;
}