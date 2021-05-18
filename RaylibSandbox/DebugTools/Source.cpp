#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include <type_traits>

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

template<typename T1, typename T2>
using LargerType_t = std::conditional_t<(sizeof(T1) > sizeof(T2)), T1, T2>;
template<typename T1, typename T2>
using LargerPrimativeType_t = std::conditional_t<(sizeof(std::remove_reference_t<T1>) > sizeof(std::remove_reference_t<T2>)), T1, T2>;

template<typename T1, typename T2>
using BestFit_t =
    std::conditional_t<
        (std::is_floating_point_v<std::remove_reference_t<T1>>),
        std::conditional_t<
            (std::is_floating_point_v<std::remove_reference_t<T2>>),
            LargerPrimativeType_t<T1, T2>, // double > float
            T1 // float > int
        >
        std::conditional_t<
            (std::is_floating_point_v<std::remove_reference_t<T2>>),
            T2, // int < float
            LargerPrimativeType_t<T1, T2> // long long > int
        >
    >;

template<typename T>
using remove_lvalue_reference_t = std::conditional_t<std::is_rvalue_reference_v<T>, T, std::remove_reference_t<T>>;

template<typename T1, typename T2>
constexpr bool is_same_primative_v = std::is_same_v<std::remove_reference_t<T1>, std::remove_reference_t<T2>>;

template<typename T1, typename T2, std::enable_if_t<is_same_primative_v<T1, T2>, bool> = true>
auto Min(T1 a, T2 b) {
    return (a < b ? a : b);
}
template<typename T1, typename T2, std::enable_if_t<!is_same_primative_v<T1, T2> && is_same_primative_v<T1, BestFit_t<T1, T2>>, bool> = true>
auto Min(T1 a, T2 b) {
    using NonRefType_t = std::remove_reference_t<T1>;
    NonRefType_t b1 = static_cast<NonRefType_t>(b1);
    return static_cast<NonRefType_t>(a < b1 ? a : b1);
}
template<typename T1, typename T2, std::enable_if_t<!is_same_primative_v<T1, T2> && !is_same_primative_v<T1, BestFit_t<T1, T2>>, bool> = true>
auto Min(T1 a, T2 b) {
    using NonRefType_t = std::remove_reference_t<T2>;
    NonRefType_t a1 = static_cast<NonRefType_t>(a);
    return static_cast<NonRefType_t>(a1 < b ? a1 : b);
}

template<typename T1, typename T2, std::enable_if_t<is_same_primative_v<T1, T2>, bool> = true>
auto Max(T1 a, T2 b) {
    return (a > b ? a : b);
}
template<typename T1, typename T2, std::enable_if_t<!is_same_primative_v<T1, T2>&& is_same_primative_v<T1, BestFit_t<T1, T2>>, bool> = true>
auto Max(T1 a, T2 b) {
    using NonRefType_t = std::remove_reference_t<T1>;
    NonRefType_t b1 = static_cast<NonRefType_t>(b1);
    return static_cast<NonRefType_t>(a > b1 ? a : b1);
}
template<typename T1, typename T2, std::enable_if_t<!is_same_primative_v<T1, T2> && !is_same_primative_v<T1, BestFit_t<T1, T2>>, bool> = true>
auto Max(T1 a, T2 b) {
    using NonRefType_t = std::remove_reference_t<T2>;
    NonRefType_t a1 = static_cast<NonRefType_t>(a);
    return static_cast<NonRefType_t>(a1 > b ? a1 : b);
}

template<typename T1, typename T2, std::enable_if_t<is_same_primative_v<T1, T2>, bool> = true>
auto Clamp(T1 a, T2 b) {
    auto b = (a < min ? min : a);
    return (b > max ? max : b);
}
template<typename T1, typename T2, std::enable_if_t<!is_same_primative_v<T1, T2>&& is_same_primative_v<T1, BestFit_t<T1, T2>>, bool> = true>
auto Clamp(T1 a, T2 b) {
    using NonRefType_t = std::remove_reference_t<T1>;
    NonRefType_t b1 = static_cast<NonRefType_t>(b1);
    return static_cast<NonRefType_t>(a < b1 ? a : b1);
}
template<typename T1, typename T2, std::enable_if_t<!is_same_primative_v<T1, T2> && !is_same_primative_v<T1, BestFit_t<T1, T2>>, bool> = true>
auto Clamp(T1 a, T2 b) {
    using NonRefType_t = std::remove_reference_t<T2>;
    NonRefType_t a1 = static_cast<NonRefType_t>(a);
    return static_cast<NonRefType_t>(a1 < b ? a1 : b);
}

template<
    typename T1,
    typename T2,
    typename T3,
    std::enable_if_t<
        std::is_arithmetic_v<std::remove_reference_t<T1>> &&
        std::is_arithmetic_v<std::remove_reference_t<T2>> &&
        std::is_arithmetic_v<std::remove_reference_t<T3>>,
        bool
    > = true
>
auto Clamp(T1 a, T2 min, T3 max)
{
    auto b = (a < min ? min : a);
    return (b > max ? max : b);
}

using Vector1 = float;

#pragma region Vector math

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

#pragma endregion

template<typename T, bool Is_Integral = std::is_integral<T>::value, bool VALID = std::is_arithmetic<T>::value>
class RangeController {};

template<typename T, bool B>
class RangeController<T, B, false>
{
    static_assert(false);
};

template<typename T>
class RangeController<T, false, true>
{
public:
    RangeController() :
        m_value(),
        m_min(), m_max()
    {}
    RangeController(T* value, T min, T max) :
        m_value(value),
        m_min(min), m_max(max)
    {}

    T GetValue() const
    {
        return *m_value;
    }
    T GetRangeMin() const
    {
        return m_min;
    }
    T GetRangeMax() const
    {
        return m_max;
    }

    void SetValue(float newValue)
    {
        *m_value = Clamp(newValue, m_min, m_max);
    }
    void SetVariable(float* newVar)
    {
        m_value = newVar;
    }
    void SetValueRange(float min, float max)
    {
        m_min = min;
        m_max = max;
    }

protected:
    T* m_value;
    T m_min, m_max;
};

template<typename T>
class RangeController<T, true, true>
{
public:
    RangeController() :
        m_value(),
        m_min(), m_max()
    {}
    RangeController(T* value, T min, T max) :
        m_value(value),
        m_min(min), m_max(max)
    {}

    T GetValue() const
    {
        return *m_value;
    }
    T GetRangeMin() const
    {
        return m_min;
    }
    T GetRangeMax() const
    {
        return m_max;
    }

    void SetValue(float newValue) const
    {
        *m_value = Clamp(newValue, m_min, m_max);
    }
    void SetVariable(float* newVar)
    {
        m_value = newVar;
    }
    void SetValueRange(float min, float max)
    {
        m_min = min;
        m_max = max;
    }

protected:
    T* m_value;
    T m_min, m_max;
};

template<typename T>
class ToggleController
{
public:
    ToggleController() :
#if _DEBUG
        m_value(nullptr),
        m_bitMask((T)(0xffffffffffffffff)),
#else
        m_value(),
        m_bitMask()
#endif
    {}
    ToggleController(T* _value, T _bitMask) :
        m_value(_value),
        m_bitMask(_bitMask)
    {}

    T GetValue() const
    {
        (*m_value) & m_bitMask;
    }
    T GetMask() const
    {
        return m_bitMask;
    }

    void ToggleValue() const
    {
        *m_value ^= m_bitMask;
    }
    void Clear() const
    {
        *m_value &= ~m_bitMask;
    }
    void Set() const
    {
        *m_value |= m_bitMask;
    }

    void SetVariable(T* newVar)
    {
        m_value = newVar;
    }
    void SetMask(T bitMask)
    {
        m_bitMask = bitMask;
    }

protected:
    T* m_value;
    T m_bitMask;
};

template<typename T>
class Slider : private RangeController<T>
{
protected:
    using Controller = RangeController<T>;

public:
    Slider() :
        Controller(),
        m_position(Vector2Zero()),
        m_handleWidth(2), m_handleLength(1),
        m_trackLength(10),
        b_horizontal(true)
    {}
    Slider(
        float* _value,
        float _min, float _max,
        Vector2 _position,
        float _handleWidth, float _handleLength,
        float _trackLength,
        bool _horizontal = true) :
        Controller(_value, _min, _max),
        m_position(_position),
        m_handleWidth(_handleWidth), m_handleLength(_handleLength),
        m_trackLength(_trackLength),
        b_horizontal(_horizontal)
    {}

    // For repositioning the slider as a whole
    void SetSliderPosition(Vector2 position)
    {
        m_position = position;
    }
    void SetHandleSize(float width, float length)
    {
        m_handleWidth = width;
        m_handleLength = length;
    }
    void SetTrackLength(float length)
    {
        m_trackLength = length;
    }
    // For dragging the handle
    void SetHandlePosition(Vector2 newPos)
    {
        float distance = Clamp((b_horizontal ? newPos.x - m_position.x : newPos.y - m_position.y) + m_handleLength * 0.5f, 0.0f, m_trackLength);
        *m_value = ValueFromDistance(distance); // The handle's position is directly based on the variable's value
    }

    float GetDistanceAlongTrack() const
    {
        return ValueToDistance();
    }
    Vector2 GetTrackStart() const
    {
        return m_position;
    }
    Vector2 GetTrackEnd() const
    {
        Vector2 end;
        if (b_horizontal)
        {
            end.x = m_position.x + m_trackLength;
            end.y = m_position.y;
        }
        else
        {
            end.x = m_position.x;
            end.y = m_position.y + m_trackLength;
        }
        return end;
    }
    Rectangle GetHandleCollision() const
    {
        Rectangle coll;
        if (b_horizontal)
        {
            coll.x = m_position.x + ValueToDistance() - m_handleLength * 0.5f;
            coll.y = m_position.y - m_handleWidth * 0.5f;
            coll.width = m_handleLength;
            coll.height = m_handleWidth;
        }
        else
        {
            coll.x = m_position.x - m_handleWidth * 0.5f;
            coll.y = m_position.y + ValueToDistance() - m_handleLength * 0.5f;
            coll.width = m_handleWidth;
            coll.height = m_handleLength;
        }
        return coll;
    }

    float ValueFromDistance(float ui) const
    {
        return ((ui / m_trackLength) * (m_max - m_min)) + m_min;
    }
    float ValueToDistance() const
    {
        return ((*m_value - m_min) / (m_max - m_min)) * m_trackLength;
    }

protected:
    Vector2 m_position;
    float m_handleWidth, m_handleLength;
    float m_trackLength;
    bool b_horizontal = true;
};
template<typename T>
void DrawSlider(Slider<T>& slider, Color color)
{
    DrawLineV(slider.GetTrackStart(), slider.GetTrackEnd(), GRAY);
    DrawRectangleRec(slider.GetHandleCollision(), color);
}

template<typename T>
class Button : private ToggleController<T>
{
protected:
    using ToggleController = ToggleController<T>;
    Vector2 m_center;
    float m_radius;
};
template<typename T>
void DrawRocker(Rocker<T>& rocker)
{
    rocker.m_offSection;
    rocker.m_onSection;
}

void DrawButon();

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    float value = 0.0f;
    Slider sliders[1];
    sliders[0] = Slider(&value, 0.0f, 15.0f, { 16.0f, 16.0f }, 16.0f, 4.0f, 128.0f, true);

    Slider* hovered = nullptr;
    Slider* held = nullptr;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/
        Vector2 mousePos = GetMousePosition();
        hovered = nullptr;
        for (Slider& slider : sliders)
        {
            if (CheckCollisionPointRec(mousePos, slider.GetHandleCollision()))
            {
                hovered = &slider;
                break;
            }
        }
        
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            held = hovered;
        }
        else if (held && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            held = nullptr;
        }

        if (held)
        {
            held->SetHandlePosition(mousePos);
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            if (held)
            {
                for (Slider& slider : sliders)
                {
                    if (&slider != held)
                        DrawSlider(slider, GRAY);
                }
                DrawSlider(*held, WHITE);
            }
            else if (hovered)
            {
                for (Slider& slider : sliders)
                {
                    if (&slider != hovered)
                        DrawSlider(slider, GRAY);
                }
                DrawSlider(*hovered, LIGHTGRAY);
            }
            else
            {
                for (Slider& slider : sliders)
                {
                    DrawSlider(slider, GRAY);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}