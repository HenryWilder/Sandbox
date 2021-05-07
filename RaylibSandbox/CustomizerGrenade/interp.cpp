#include <cmath>
#include "interp.h"

double sterp(double a, double b, double t) noexcept {
	return (t < 0.5 ? a : b);
}

namespace {
	inline double _lerp(double a, double b, double t) noexcept {
		return a + t * (b - a);
	}

	constexpr double c_pi = 3.141592653589793;
	inline double _coserpt(double t) noexcept {
		return -cos(c_pi * t) / 2.0 + 0.5;
	}

	inline double _smerpt(double t) noexcept {
		return t * t * (3.0 - 2.0 * t);
	}

	inline double _accerpt(double t) noexcept {
		return t * t;
	}

	inline double _decerpt(double t) noexcept {
		double tinv = 1.0 - t;
		return 1.0 - tinv * tinv;
	}
}

double lerp(double a, double b, double t) noexcept {
	return ::_lerp(a, b, t);
}

double coserp(double a, double b, double t) noexcept {
	return ::_lerp(a, b, ::_coserpt(t));
}

double smerp(double a, double b, double t) noexcept {
	return ::_lerp(a, b, ::_smerpt(t));
}

double accerp(double a, double b, double t) noexcept {
	return ::_lerp(a, b, ::_accerpt(t));
}

double decerp(double a, double b, double t) noexcept {
	return ::_lerp(a, b, ::_decerpt(t));
}