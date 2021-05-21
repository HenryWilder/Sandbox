#if !(defined(MATH_H_BASE) && defined(MATH_H_RAYLIB))

// Raylib
#if defined(RAYMATH_H) && !defined(MATH_H_RAYLIB)

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#endif

#pragma region Range

// Base
#if !defined(MATH_H_BASE)

float ScaleFromRange(float min, float max);
float RangeToScalar(float value, float from_min, float from_max);
float RangeFromScalar(float value, float to_min, float to_max);
float RangeRemap(float value, float from_min, float from_max, float to_min, float to_max);

#endif // MATH_H_BASE

// Raylib
#if defined(RAYMATH_H) && !defined(MATH_H_RAYLIB)

Vector2 ScaleFromRangeVector2(Vector2 min, Vector2 max);
Vector2 RangeToScalarVector2(Vector2 value, Vector2 from_min, Vector2 from_max);
Vector2 RangeFromScalarVector2(Vector2 value, Vector2 to_min, Vector2 to_max);
Vector2 RangeRemapVector2(Vector2 value, Vector2 from_min, Vector2 from_max, Vector2 to_min, Vector2 to_max);

Vector3 ScaleFromRangeVector3(Vector3 min, Vector3 max);
Vector3 RangeToScalarVector3(Vector3 value, Vector3 from_min, Vector3 from_max);
Vector3 RangeFromScalarVector3(Vector3 value, Vector3 to_min, Vector3 to_max);
Vector3 RangeRemapVector3(Vector3 value, Vector3 from_min, Vector3 from_max, Vector3 to_min, Vector3 to_max);

#endif // RAYMATH_H

#pragma endregion

#pragma region Footer

// Base
#ifndef MATH_H_BASE
#define MATH_H_BASE
#endif

// Raylib
#ifndef MATH_H_RAYLIB
#define MATH_H_RAYLIB
#endif

#pragma endregion

#endif