#pragma once

double sterp (double a, double b, double t) noexcept;
double lerp  (double a, double b, double t) noexcept;
double coserp(double a, double b, double t) noexcept;
double smerp (double a, double b, double t) noexcept;
double accerp(double a, double b, double t) noexcept;
double decerp(double a, double b, double t) noexcept;

using InterpDelegate_Const = double(*)(double a, double b, double t);

enum class Interp {
	Step,
	Linear,
	Cosine,
	Smooth,
	Accel,
	Decel,
};

struct InterpDelegate {
	void operator=(InterpDelegate_Const _f) {
		f = _f;
	}

	inline static InterpDelegate_Const select(Interp _fd) {
		switch (_fd)
		{
		case Interp::Step:   return sterp;
		case Interp::Linear: return lerp;
		case Interp::Cosine: return coserp;
		case Interp::Smooth: return smerp;
		case Interp::Accel:  return accerp;
		case Interp::Decel:  return decerp;
		}
	}
	void operator=(Interp _f) {
		f = select(_f);
	}

	double operator()(double a, double b, double t) const {
		return (f(a, b, t));
	}

	template<typename T>
	void interp(T& c, T a, T b, double t) const {
		c = static_cast<T>((f(static_cast<double>(a), static_cast<double>(b), t)));
	}

	template<size_t NUM, typename T>
	void interp_arr(T(&c)[NUM], const T(&&a)[NUM], const T(&&b)[NUM], double t) const {
		for (size_t i = 0; i < NUM; ++i) {
			c[i] = static_cast<T>((f(static_cast<double>(a[i]), static_cast<double>(b[i]), t)));
		}
	}
	template<size_t NUM, typename T>
	void interp_arr(T(&c)[NUM], const T(&a)[NUM], const T(&b)[NUM], double t) const {
		for (size_t i = 0; i < NUM; ++i) {
			c[i] = static_cast<T>((f(static_cast<double>(a[i]), static_cast<double>(b[i]), t)));
		}
	}

#ifdef RAYLIB_H
	void interpV2(Vector2& c, Vector2 a, Vector2 b, double t) const {
		c.x = (float)(f(a.x, b.x, t));
		c.y = (float)(f(a.y, b.y, t));
	}
	void interpV3(Vector3& c, Vector3 a, Vector3 b, double t) const {
		c.x = (float)(f(a.x, b.x, t));
		c.y = (float)(f(a.y, b.y, t));
		c.z = (float)(f(a.z, b.z, t));
	}
	void interpC(Color& c, Color a, Color b, double t) const {
		c.r = (unsigned char)(f(a.r, b.r, t));
		c.g = (unsigned char)(f(a.g, b.g, t));
		c.b = (unsigned char)(f(a.b, b.b, t));
		c.a = (unsigned char)(f(a.a, b.a, t));
	}
	void interpRec(Rectangle& c, Rectangle a, Rectangle b, double t) const {
		c.x = (f(a.x, b.x, t));
		c.y = (f(a.y, b.y, t));
		c.width = (f(a.width, b.width, t));
		c.height = (f(a.height, b.height, t));
	}
#endif // RAYLIB_H

private:
	InterpDelegate_Const f;
};