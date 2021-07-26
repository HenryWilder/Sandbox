#pragma once
#if 0
#include <array>
struct Pos {
	Pos() : x(), y() {};
	Pos(int _x, int _y) : x(_x), y(_y) {};

	int x, y;

	Pos operator+(Pos b) { return Pos(x + b.x, y + b.y); }
	Pos operator-(Pos b) { return Pos(x - b.x, y - b.y); }
};

template<class T, int width, int height>
class Array2D
{
public:
	constexpr int GetWidth()  const { return width; }
	constexpr int GetHeight() const { return height; }
	constexpr int GetArea()   const { return width * height; }
	constexpr int size()	  const { return GetArea(); }

	Array2D(const std::array<T, GetArea()> in) {
		for (int i = 0; i < GetArea(); ++i) {
			elements[i] = in[i];
		}
	}
	Array2D(const T (&in)[GetArea()]) {
		for (int i = 0; i < GetArea(); ++i) {
			elements[i] = in[i];
		}
	}
	Array2D(const T* in) {
		for (int i = 0; i < GetArea(); ++i) {
			elements[i] = in[i];
		}
	}

	static int Index(int _x, int _y) { return (_y * width) + _x; }

	T& operator[](int _x, int _y) { return elements[Index(_x, _y)]; }

	struct Section {
		Section() : x(0), y(0), w(width), h(height) {};
		Section(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {};

		int x, y, w, h;
	};

	class iter
	{
	public:
		iter() : pt(), elem(), sec() {};
		iter(Pos _pt, T* _elem, Section _sec) : pt(_pt), elem(_elem), sec(_sec) {};

	protected:
		Pos pt;
		T* elem;
		Section sec;

	public:
		T& operator*() { return *elem; }
		T* operator->() { return elem; }

		iter operator++() {
			iter next = *this;
			next.elem++;
			next.pt.x++;
			if (next.pt.x > (sec.x + sec.width))
			{
				pt.x = sec.x;
				pt.y++;
			}
			return next;
		}

		bool operator!=(iter b) { return ( (pt.y != b.pt.y) || (pt.x != b.pt.x) ); }
	};
	iter begin()
	{
		return iter(Pos(0,0), elements, Section(0, 0, GetWidth(), GetHeight()));
	}
	iter end()
	{
		return iter(Pos(GetWidth(), GetHeight()), elements + GetArea(), Section(0, 0, GetWidth(), GetHeight()));
	}

	struct iter_scanline : iter
	{
		iter_scanline() : iter() {};
		iter_scanline(Pos _pt, T* _elem, Section _sec) : iter(_pt, _elem, _sec) {};

		iter_scanline operator++()
		{

		}
	};
	struct Scanline
	{
		Scanline()
		{

		}

		Array2D& arr;
		Section sec;

		iter_scanline begin()
		{
			return iter_scanline(arr[0]);
		}
		iter_scanline end()
		{
			return iter_scanline(arr[0] + arr.size());
		}
	};

	struct iter_hilbert : iter
	{
		//rotate/flip a quadrant appropriately
		void rot(int n, int* x, int* y, int rx, int ry) {
			if (ry == 0) {
				if (rx == 1) {
					*x = n - 1 - *x;
					*y = n - 1 - *y;
				}

				//Swap x and y
				int t = *x;
				*x = *y;
				*y = t;
			}
		}

		//convert (x,y) to d
		int xy2d(int n, int x, int y) {
			int rx, ry, s, d = 0;
			for (s = n / 2; s > 0; s /= 2) {
				rx = (x & s) > 0;
				ry = (y & s) > 0;
				d += s * s * ((3 * rx) ^ ry);
				rot(n, &x, &y, rx, ry);
			}
			return d;
		}

		//convert d to (x,y)
		Pos d2xy(int n, int d)
		{
			int t = d;
			Pos pt = Pos(0,0);
			for (int s = 1; s < n; s *= 2)
			{
				int rx = (t / 2) & 1;
				int ry = (t ^ rx) & 1;
				rot(s, x, y, rx, ry);
				pt.x += s * rx;
				pt.y += s * ry;
				t /= 4;
			}
			return pt;
		}
	};
	struct Hilbert
	{

	};
	iter_hilbert Hilbert()
	{

	}

	struct iter_radiate : iter
	{

	};
	struct radiate
	{

	};
	iter_radiate Radiate()
	{

	}

private:
	T elements[width * height];
};

#include <vector>
struct Vector2D
{
};
#endif