#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Transistor;

enum class WireDirection : char
{
    XFirst = 0,
    YFirst,
    DiagStart,
    DiagEnd,

    Size // Total directions (for %)
};

Vector2 JointPos(Vector2 start, Vector2 end, WireDirection direction);

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction);
void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction, int width);

bool Vector2Equal(const Vector2& a, const Vector2& b);

// Just transmits signals from start to end
struct Wire
{
    Wire() : inTransistor{ nullptr }, outTransistor{ nullptr }, direction{ WireDirection::XFirst }, active{ false } {};
    Wire(Transistor* _inTransistor, Transistor* _outTransistor, WireDirection _direction) : inTransistor{ _inTransistor }, outTransistor{ _outTransistor }, direction{ _direction }, active{ false } {};
    Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction);

    static std::vector<Wire*> allWires;
    Transistor* inTransistor;
    Transistor* outTransistor;
    WireDirection direction;
    bool active;

    Vector2 GetStartPos() const;
    Vector2 GetEndPos() const;
    Vector2 GetJointPos() const;
    float GetLength() const;

    bool IsPointOnLine(Vector2 point) const;

    void Draw() const;
    void Highlight(Color color, float width) const;

    void SearchConnectableTransistors(Vector2 startPos, Vector2 endPos);
    void ClearReferences();
};

extern std::vector<Wire*> allWires;
