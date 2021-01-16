#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Transistor;

enum class WireDirection : char
{
    XFirst = 0,
    YFirst,
    Direct,
};

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction);

bool Vector2Equal(const Vector2& a, const Vector2& b);

// Just transmits signals from start to end
struct Wire
{
    Wire() : startPos{}, endPos{}, direction{}, active{} {};

    Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction, std::vector<Transistor*>* transistorArr);

    ~Wire();

    Vector2 startPos, endPos;
    Transistor* inTransistor;
    Transistor* outTransistor;
    WireDirection direction;
    bool active;

    void Draw();

    void SearchConnectableTransistors(std::vector<Transistor*>* transistorArr);
};