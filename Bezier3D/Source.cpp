#include "Math.h"
#include "Rendering.h"
#include "Interp.h"

int main()
{
	//WTri2Q qTri0 = WTri2Q{
	//	Vector2{ 200, 200 }, // p0
	//	Vector2{ 250, 250 },
	//	Vector2{ 300, 300 }, // p1
	//	Vector2{ 200, 350 },
	//	Vector2{ 100, 300 }, // p2
	//	Vector2{ 200, 300 }
	//};
	//DrawWTri2Q(qTri0, 0.025f);
	
	//WTri2Q qTri1 = WTri2Q{
	//	Vector2{ 200, 200 }, // p0
	//	Vector2{ 300, 150 },
	//	Vector2{ 400, 200 }, // p1
	//	Vector2{ 300, 200 },
	//	Vector2{ 300, 300 }, // p2
	//	Vector2{ 250, 250 },
	//};
	//DrawWTri2Q(qTri1, 0.025f);

	DrawTriangle({200,200}, {300,300}, {100,300}, { 255,0,0 });

	return 0;
}
