#include <Windows.h>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>

int Clamp(const int value, const int min, const int max)
{
	if (value < min) return min;
	else if (value > max) return max;
	else return value;
}

struct Coord
{
	double x, y;
	operator POINT() const { return { (LONG)x, (LONG)y }; }
	Coord operator+(const Coord p1) const { return { x + p1.x, y + p1.y }; }
	Coord operator-(const Coord p1) const { return { x - p1.x, y - p1.y }; }
	Coord operator*(const double scale) const { return { x * scale, y * scale }; }
	Coord operator/(const double scale) const { return { x / scale, y / scale }; }
	double Length() const { return sqrt(x * x + y * y); }
	Coord Normal() const { return { x / Length(), y / Length() }; }
};

struct Vec3
{
	double x, y, z;
	Vec3 operator+(const Vec3 p1) const { return { x + p1.x, y + p1.y, z + p1.z }; }
	Vec3 operator-(const Vec3 p1) const { return { x - p1.x, y - p1.y, z - p1.z }; }
	Vec3 operator*(const double scale) const { return { x * scale, y * scale, z * scale }; }
	Vec3 operator/(const double scale) const { return { x / scale, y / scale, z / scale }; }
	double Length() const { return sqrt(x * x + y * y + z * z); }
	Vec3 Normal() const { return { x / Length(), y / Length(), z / Length() }; }
	Coord Trunc() const { return { x, y }; }
};


template<int size>
struct Matrix
{
	Matrix
};

double Dist(const Coord p1, const Coord p2) { return (p2 - p1).Length(); }
bool InCircle(const Coord pos, const Coord origin, double radius) { return radius >= Dist(pos, origin); }
bool InCircle(const Vec3 pos, const Vec3 origin, double radius) { return InCircle(Coord{ pos.x, pos.y }, Coord{ origin.x, origin.y }, radius); }
double Dist(const Vec3 p1, const Vec3 p2) { return (p2 - p1).Length(); }
bool InSphere(const Vec3 pos, const Vec3 origin, double radius) { return radius >= Dist(pos, origin); }

struct Color
{
	Color() : r{ 0 }, g{ 0 }, b{ 0 } {};
	Color(unsigned char gray) : r{ gray }, g{ gray }, b{ gray } {};
	Color(unsigned char _r, unsigned char _g, unsigned char _b) : r{ _r }, g{ _g }, b{ _b } {};

	unsigned char r, g, b;

	operator COLORREF() const { return RGB(r, g, b); }
};

template<int WIDTH, int HEIGHT>
class Image
{
public:
	~Image() { delete [] pixel; }

private:
	Color* pixel = new Color[WIDTH * HEIGHT];

	size_t Index(const int x, const int y) const { return (size_t(y) * size_t(WIDTH)) + size_t(x); }

public:
	Color Get(const int x, const int y) { return pixel[Index(x,y)]; }
	void Set(const int x, const int y, const Color color) { pixel[Index(x,y)] = color; }
};

Vec3 ProjectToSphereSurface(const Vec3 pos, const double radius) { return { pos.x, pos.y, sqrt((radius * radius) - (pos.x * pos.x + pos.y * pos.y)) }; }

Vec3 LightBlobPoint(const Vec3 origin, const double radius, const Vec3 lightPos)
{
	Vec3 directionToLight = (lightPos - origin).Normal();
	Vec3 closestSurfacePoint = ProjectToSphereSurface(directionToLight, radius) * radius;
	Vec3 pointInSpace = origin + closestSurfacePoint;
	return pointInSpace;
}

struct SceneData
{
	Vec3 light;
	Vec3 origin;
	double radius;

	Vec3 lightBlobPoint;
};

SceneData g_scene = { { 200,32,100 }, { 128,128,0 }, 64, 0 };

typedef Color(*Shader)(int x, int y);

template<int width, int height>
Color BoxBlurFragment(int x, int y, Image<width,height>* samplespace)
{
	Color out;
	int sum;
	sum =
		samplespace->Get(x - 1, y + 1).r + // Top left
		samplespace->Get(x + 0, y + 1).r + // Top center
		samplespace->Get(x + 1, y + 1).r + // Top right
		samplespace->Get(x - 1, y + 0).r + // Mid left
		samplespace->Get(x + 0, y + 0).r + // Current pixel
		samplespace->Get(x + 1, y + 0).r + // Mid right
		samplespace->Get(x - 1, y - 1).r + // Low left
		samplespace->Get(x + 0, y - 1).r + // Low center
		samplespace->Get(x + 1, y - 1).r;  // Low right
	out.r = (unsigned char)(sum / 9);
	sum =
		samplespace->Get(x - 1, y + 1).g + // Top left
		samplespace->Get(x + 0, y + 1).g + // Top center
		samplespace->Get(x + 1, y + 1).g + // Top right
		samplespace->Get(x - 1, y + 0).g + // Mid left
		samplespace->Get(x + 0, y + 0).g + // Current pixel
		samplespace->Get(x + 1, y + 0).g + // Mid right
		samplespace->Get(x - 1, y - 1).g + // Low left
		samplespace->Get(x + 0, y - 1).g + // Low center
		samplespace->Get(x + 1, y - 1).g;  // Low right
	out.g = (unsigned char)(sum / 9);
	sum =
		samplespace->Get(x - 1, y + 1).b + // Top left
		samplespace->Get(x + 0, y + 1).b + // Top center
		samplespace->Get(x + 1, y + 1).b + // Top right
		samplespace->Get(x - 1, y + 0).b + // Mid left
		samplespace->Get(x + 0, y + 0).b + // Current pixel
		samplespace->Get(x + 1, y + 0).b + // Mid right
		samplespace->Get(x - 1, y - 1).b + // Low left
		samplespace->Get(x + 0, y - 1).b + // Low center
		samplespace->Get(x + 1, y - 1).b;  // Low right
	out.b = (unsigned char)(sum / 9);
	return out;
}

template<int width, int height>
Color GaussianBlurFragment(int x, int y, Image<width, height>* samplespace)
{
	constexpr int conv = 24;
	constexpr double dConv = (double)conv;
	constexpr int convHalf = conv / 2;

	Color out;

	double sum = 0.0;

	for (int cy = 0; cy < conv; ++cy) {
		for (int cx = 0; cx < conv; ++cx)
		{
			unsigned char val = samplespace->Get(x + cx - convHalf, y + cy - convHalf).r;
			int weight = ((convHalf - abs(cx - convHalf)) + (convHalf - abs(cy - convHalf)));
			double c = (double)weight / (dConv * 2.0);
			sum += c * (double)val;
		}
	}
	out.r = (unsigned char)(sum / (dConv * dConv));

	sum = 0.0;
	for (int cy = 0; cy < conv; ++cy) {
		for (int cx = 0; cx < conv; ++cx)
		{
			unsigned char val = samplespace->Get(x + cx - convHalf, y + cy - convHalf).g;
			int weight = ((convHalf - abs(cx - convHalf)) + (convHalf - abs(cy - convHalf)));
			double c = (double)weight / (dConv * 2.0);
			sum += c * (double)val;
		}
	}
	out.g = (unsigned char)(sum / (dConv * dConv));

	sum = 0.0;
	for (int cy = 0; cy < conv; ++cy) {
		for (int cx = 0; cx < conv; ++cx)
		{
			unsigned char val = samplespace->Get(x + cx - convHalf, y + cy - convHalf).b;
			int weight = ((convHalf - abs(cx - convHalf)) + (convHalf - abs(cy - convHalf)));
			double c = (double)weight / (dConv * 2.0);
			sum += c * (double)val;
		}
	}
	out.b = (unsigned char)(sum / (dConv * dConv));

	return out;
}

Color LightBlobFragment(int x, int y)
{
	Vec3 pix = {(double)x, (double)y, 0};
	Color pixelColor(0, 0, 0);

	if (InCircle(pix, g_scene.origin, g_scene.radius))
	{
		pixelColor = Color(127);

		if (InCircle(pix, g_scene.lightBlobPoint, 16))
		{
			double val = 255.0 - 255.0 * (Dist(pix.Trunc(), g_scene.lightBlobPoint.Trunc()) / 16.0);
			val *= 0.5;

			if (val < (255 - pixelColor.r)) pixelColor = Color(pixelColor.r + (unsigned char)val);
			else pixelColor = Color(255);
		}
	}

	if (InCircle(pix, g_scene.light, 3)) pixelColor = Color(255, 255, 0);

	return pixelColor;
}

template<int width, int height>
void ApplyShader(const Shader fragment, Image<width, height>* buffer, RECT region = {0, 0, width, height})
{
	for (int y = region.top; y < region.bottom; ++y) {
		for (int x = region.left; x < region.right; ++x) {
			buffer->Set(x, y, fragment(x, y));
		}
	}
}

template<int width, int height>
void ApplyPostProcess(Color (*fragment)(int, int, Image<width, height>*), Image<width, height>* input, Image<width, height>* output)
{
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			output->Set(x, y, fragment(x, y, input));
		}
	}
}

template<int width, int height>
void DrawBuffer(HDC context, Image<width, height>* buffer, RECT region = { 0, 0, width, height })
{
	for (int y = region.top; y < region.bottom; ++y) {
		for (int x = region.left; x < region.right; ++x) {
			SetPixelV(context, x, y, buffer->Get(x,y));
		}
	}
}

int main()
{
	HWND console = GetConsoleWindow();
	HDC hdc = GetDC(console);

	Image<127, 127> buffer;
	Image<127, 127> frame;

	g_scene.lightBlobPoint = LightBlobPoint(g_scene.origin, g_scene.radius, g_scene.light);

	{
		std::thread t0(ApplyShader<255, 255>, LightBlobFragment, &frame, RECT{ 0,0,127,127 });
		std::thread t1(ApplyShader<255, 255>, LightBlobFragment, &frame, RECT{ 128,0,255,127 });
		std::thread t2(ApplyShader<255, 255>, LightBlobFragment, &frame, RECT{ 0,128,127,255 });
		std::thread t3(ApplyShader<255, 255>, LightBlobFragment, &frame, RECT{ 128,128,255,255 });

		t0.join();
		t1.join();
		t2.join();
		t3.join();

		ApplyPostProcess(GaussianBlurFragment<255, 255>, &buffer, &frame);

		t0 = std::thread(DrawBuffer<255, 255>, hdc, &frame, RECT{ 0,0,127,127 });
		t1 = std::thread(DrawBuffer<255, 255>, hdc, &frame, RECT{ 128,0,255,127 });
		t2 = std::thread(DrawBuffer<255, 255>, hdc, &frame, RECT{ 0,128,127,255 });
		t3 = std::thread(DrawBuffer<255, 255>, hdc, &frame, RECT{ 128,128,255,255 });
	}


	//ApplyShader(LightBlobFragment, &buffer);

	//ApplyPostProcess(GaussianBlurFragment<255,255>, &buffer, &frame);

	//DrawBuffer(hdc, &frame);

	
	
	
	

	ReleaseDC(console, hdc);

	std::cin.ignore();

	return 0;
}
