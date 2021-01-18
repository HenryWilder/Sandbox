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

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction);
void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction, int width);

bool Vector2Equal(const Vector2& a, const Vector2& b);

// Just transmits signals from start to end
struct Wire
{
    Wire() : inTransistor{ nullptr }, outTransistor{ nullptr }, direction{ (WireDirection)(NULL) }, active{ false } {};

    Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction, std::vector<Transistor*>* transistorArr);

    ~Wire();

    Transistor* inTransistor;
    Transistor* outTransistor;
    WireDirection direction;
    bool active;

    Vector2 GetStartPos() const;
    Vector2 GetEndPos() const;

    void Draw() const;
    void Highlight(Color color, int width) const;

    void SearchConnectableTransistors(Vector2 startPos, Vector2 endPos, std::vector<Transistor*>* transistorArr);
};