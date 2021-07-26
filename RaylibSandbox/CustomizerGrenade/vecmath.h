#pragma once
#ifndef _FVEC_H_INCLUDED
#include <fvec.h>
#endif

#ifndef RAYLIB_H
#include "raylib.h" // Required for structs: Vector3, Rectangle, Color
#endif
#ifndef RAYMATH_H
#include <raymath.h>
#endif


#define _VECM_START namespace { namespace vec {
#define _VECM_END } }
#define _VECM ::vec::

_VECM_START
inline float _lerp(float y0, float y1, float t) noexcept { return y0 + t * (y1 - y0); }
_VECM_END


#pragma region Vector math

// Vector2

inline Vector2  operator- (Vector2  vec)          noexcept { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) noexcept { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) noexcept { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) noexcept { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) noexcept { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) noexcept { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) noexcept { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) noexcept { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) noexcept { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) noexcept { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) noexcept { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) noexcept { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) noexcept { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) noexcept { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) noexcept { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) noexcept { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) noexcept { return (a = Vector2Scale(a, 1.0f / div));   }

struct Flt2Ref {
	Flt2Ref(Vector2& vec)
		: x(&vec.x), y(&vec.y) {}
	Flt2Ref(float& _x, float& _y)
		: x(&_x), y(&_y) {}
	Flt2Ref(float (&arr)[2])
		: x(arr), y(arr + 1) {}

	float* const x;
	float* const y;

	operator Vector2() const { return { *x,*y }; }

	Flt2Ref& operator=(Vector2 v) {
		*x = v.x;
		*y = v.y;
		return *this;
	}

	Flt2Ref& operator+=(Vector2 b) { return (*this = (*this + b)); }
	Flt2Ref& operator+=(float v)   { return (*this = (*this + v)); }
	Flt2Ref& operator-=(Vector2 b) { return (*this = (*this - b)); }
	Flt2Ref& operator-=(float v)   { return (*this = (*this - v)); }
	Flt2Ref& operator*=(Vector2 b) { return (*this = (*this * b)); }
	Flt2Ref& operator*=(float s)   { return (*this = (*this * s)); }
	Flt2Ref& operator/=(Vector2 b) { return (*this = (*this / b)); }
	Flt2Ref& operator/=(float d)   { return (*this = (*this / d)); }
};

// Vector3

inline Vector3  operator- (Vector3  vec)          noexcept { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) noexcept { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) noexcept { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) noexcept { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) noexcept { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) noexcept { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) noexcept { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) noexcept { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) noexcept { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) noexcept { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) noexcept { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) noexcept { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) noexcept { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) noexcept { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) noexcept { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) noexcept { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) noexcept { return (a = Vector3Scale(a, 1.0f / div));   }

struct Flt3Ref {
	Flt3Ref(Vector3& vec)
		: x(&vec.x), y(&vec.y), z(&vec.z) {}
	Flt3Ref(Vector2& vec, float& _z)
		: x(&vec.x), y(&vec.y), z(&_z) {}

	Flt3Ref(float& _x, float& _y, float& _z)
		: x(&_x), y(&_y), z(&_z) {}

	Flt3Ref(float (&arr)[3])
		: x(arr), y(arr + 1), z(arr + 2) {}

	Flt3Ref(float arr[3])
		: x(arr), y(arr + 1), z(arr + 2) {}

	Flt3Ref(float* arr[3])
		: x(arr[0]), y(arr[1]), z(arr[2]) {}

	float* const x;
	float* const y;
	float* const z;

	operator Vector3() { return { *x,*y,*z }; }

	Flt3Ref& operator=(Vector3 v) {
		*x = v.x;
		*y = v.y;
		*z = v.z;
		return *this;
	}
	Flt3Ref& operator+=(Vector3 b) { return (*this = (*this + b)); }
	Flt3Ref& operator+=(float v)   { return (*this = (*this + v)); }
	Flt3Ref& operator-=(Vector3 b) { return (*this = (*this - b)); }
	Flt3Ref& operator-=(float v)   { return (*this = (*this - v)); }
	Flt3Ref& operator*=(Vector3 b) { return (*this = (*this * b)); }
	Flt3Ref& operator*=(float s)   { return (*this = (*this * s)); }
	Flt3Ref& operator/=(Vector3 b) { return (*this = (*this / b)); }
	Flt3Ref& operator/=(float d)   { return (*this = (*this / d)); }
};

#pragma endregion

#pragma region Rectangle math

inline Flt2Ref RecSize(Rectangle& rec) noexcept { return Flt2Ref{ rec.width, rec.height };	}
inline Flt2Ref RecPos (Rectangle& rec) noexcept { return Flt2Ref{ rec.x,		rec.y };	}

inline float   RecLeft			(Rectangle rec) noexcept { return rec.x;								}
inline float   RecRight			(Rectangle rec) noexcept { return rec.x + rec.width;					}
inline float   RecTop			(Rectangle rec) noexcept { return rec.y;								}
inline float   RecBottom		(Rectangle rec) noexcept { return rec.y + rec.height;					}

inline float   RecHalfWidth		(Rectangle rec) noexcept { return rec.width  * 0.5f;					}
inline float   RecHalfHeight	(Rectangle rec) noexcept { return rec.height * 0.5f;					}

inline Vector2 RecRange			(Rectangle rec) noexcept { return { rec.width, rec.height };			}
inline Vector2 RecHalfSize		(Rectangle rec) noexcept { return RecRange(rec) * 0.5f;					}

inline Vector2 RecTopLeft		(Rectangle rec) noexcept { return { rec.x,			rec.y };			}
inline Vector2 RecTopRight		(Rectangle rec) noexcept { return { RecRight(rec),	rec.y };			}
inline Vector2 RecBottomLeft	(Rectangle rec) noexcept { return { rec.x,			RecBottom(rec) };	}
inline Vector2 RecBottomRight	(Rectangle rec) noexcept { return { RecRight(rec),	RecBottom(rec) };	}

inline float   RecMidH			(Rectangle rec) noexcept { return rec.x + RecHalfWidth(rec);			}
inline float   RecMidV			(Rectangle rec) noexcept { return rec.y + RecHalfHeight(rec);			}
inline Vector2 RecMid			(Rectangle rec) noexcept { return RecTopLeft(rec) + RecHalfSize(rec);	}

#pragma endregion

#pragma region Color math

_VECM_START
constexpr float scaleFactor = 255.0f;

inline float _saturate(float value) noexcept { return (value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value)); }
inline float _toFlt(unsigned char c) { return static_cast<float>(c) / scaleFactor; }
inline unsigned char _toComponent(float f) noexcept { return static_cast<unsigned char>(roundf(_saturate(f) * scaleFactor)); }
_VECM_END

// Interpolates between two colors
inline Color ColorCombine(Color a, Color b, float t) noexcept {
	Color out;
#if defined _FVEC_H_INCLUDED && 0
	const F32vec4 scale{ _VECM scaleFactor };
	const F32vec4 scaleInv{ 1.0f / _VECM scaleFactor };
	F32vec4 a1{ (float)(a.r), (float)(a.g), (float)(a.b), (float)(a.a) }; a1 *= scaleInv;
	F32vec4 b1{ (float)(b.r), (float)(b.g), (float)(b.b), (float)(b.a) }; b1 *= scaleInv;
	F32vec4 t1 = F32vec4{ t };
	a1 += t1 * (b1 - a1); // Lerp
	a1 *= scale;
	out = {
		static_cast<unsigned char>(roundf(_VECM _saturate(a1[0]))),
		static_cast<unsigned char>(roundf(_VECM _saturate(a1[1]))),
		static_cast<unsigned char>(roundf(_VECM _saturate(a1[2]))),
		static_cast<unsigned char>(roundf(_VECM _saturate(a1[3])))
	};
#else // !_FVEC_H_INCLUDED
	out = {
		_VECM _toComponent(_VECM _lerp(_VECM _toFlt(a.r), _VECM _toFlt(b.r), t)),
		_VECM _toComponent(_VECM _lerp(_VECM _toFlt(a.g), _VECM _toFlt(b.g), t)),
		_VECM _toComponent(_VECM _lerp(_VECM _toFlt(a.b), _VECM _toFlt(b.b), t)),
		_VECM _toComponent(_VECM _lerp(_VECM _toFlt(a.a), _VECM _toFlt(b.a), t))
	};
#endif
	return out;
}

inline Color HueShift(Color color, float amount) {
	Vector3 hsv = ColorToHSV(color);
	return ColorFromHSV(hsv.x + amount, hsv.y, hsv.z);
}

inline Color Saturation(Color color, float scale) {
	Vector3 hsv = ColorToHSV(color);
	return ColorFromHSV(hsv.x, hsv.y * scale, hsv.z);
}

inline Color Brightness(Color color, float scale) {
	Vector3 hsv = ColorToHSV(color);
	return ColorFromHSV(hsv.x, hsv.y, hsv.z * scale);
}

#pragma endregion

#undef _VECM
#undef _VECM_START
#undef _VECM_END