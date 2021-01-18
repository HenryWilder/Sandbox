#include "Wire.h"
#include "Transistor.h"

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction)
{
    switch (direction)
    {
    default: case WireDirection::XFirst:
        DrawLineV(start, { end.x, start.y }, color);
        DrawLineV({ end.x, start.y }, end, color);
        break;
    case WireDirection::YFirst:
        DrawLineV(start, { start.x, end.y }, color);
        DrawLineV({ start.x, end.y }, end, color);
        break;
    case WireDirection::DiagStart:
    {
        float shortestLength = __min(abs(end.x - start.x), abs(end.y - start.y));
        Vector2 angledPos = { start.x + ((end.x > start.x ? 1.0f : -1.0f) * shortestLength), start.y + ((end.y > start.y ? 1.0f : -1.0f) * shortestLength) };
        DrawLineV(start, angledPos, color);
        DrawLineV(angledPos, end, color);
    }
        break;
    case WireDirection::DiagEnd:
    {
        float shortestLength = __min(abs(end.x - start.x), abs(end.y - start.y));
        Vector2 angledPos = { end.x - ((end.x > start.x ? 1.0f : -1.0f) * shortestLength), end.y - ((end.y > start.y ? 1.0f : -1.0f) * shortestLength) };
        DrawLineV(start, angledPos, color);
        DrawLineV(angledPos, end, color);
    }
    break;
    }
}

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction, int width)
{
    for (int x = ((-1 * width) / 2); x < (width / 2); ++x)
    {
        for (int y = ((-1 * width) / 2); y < (width / 2); ++y)
        {
            DrawSnappedLine({ start.x + (float)x, start.y + (float)y }, { end.x + (float)x, end.y + (float)y }, color, direction);
        }
    }
}

bool Vector2Equal(const Vector2& a, const Vector2& b)
{
    return
        abs(a.x - b.x) < 2.0f &&
        abs(a.y - b.y) < 2.0f;
}

Vector2 Wire::GetStartPos() const
{
    return inTransistor->pos;
}

Vector2 Wire::GetEndPos() const
{
    return outTransistor->pos;
}

void Wire::SearchConnectableTransistors(Vector2 startPos, Vector2 endPos, std::vector<Transistor*>* transistorArr)
{
    for (Transistor* searchStart : *transistorArr)
    {
        if (Vector2Equal(searchStart->pos, startPos))
        {
            inTransistor = searchStart;
            searchStart->outputs.push_back(this);
            break;
        }
    }
    for (Transistor* searchEnd : *transistorArr)
    {
        if (Vector2Equal(searchEnd->pos, endPos))
        {
            outTransistor = searchEnd;
            searchEnd->inputs.push_back(this);
            break;
        }
    }
}

Wire::Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction, std::vector<Transistor*>* transistorArr)
{
    direction = _direction;

    inTransistor = nullptr;
    outTransistor = nullptr;

    SearchConnectableTransistors(_startPos, _endPos, transistorArr);

    if (!inTransistor)
    {
        inTransistor = new Transistor(_startPos);
        inTransistor->outputs.push_back(this);
        transistorArr->push_back(inTransistor);
    }
    if (!outTransistor)
    {
        outTransistor = new Transistor(_endPos);
        outTransistor->inputs.push_back(this);
        transistorArr->push_back(outTransistor);
    }

    active = false;
}

Wire::~Wire()
{
    for (int i = 0; i < inTransistor->outputs.size(); ++i)
    {
        if (inTransistor->outputs[i] == this) inTransistor->outputs.erase(inTransistor->outputs.begin() + i);
    }
    
    for (int i = 0; i < outTransistor->inputs.size(); ++i)
    {
        if (outTransistor->inputs[i] == this) outTransistor->inputs.erase(outTransistor->inputs.begin() + i);
    }
}

void Wire::Draw() const
{
    Color color;
    if (this->active) color = RED;
    else color = WHITE;
    DrawSnappedLine(inTransistor->pos, outTransistor->pos, color, direction);
}

void Wire::Highlight(Color color, int width) const
{
    DrawSnappedLine(inTransistor->pos, outTransistor->pos, color, direction, width);
}
