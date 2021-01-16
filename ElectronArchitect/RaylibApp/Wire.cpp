#include "Wire.h"
#include "Transistor.h"

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction)
{
    switch (direction)
    {
    case WireDirection::XFirst:
        DrawLineV(start, { start.x, end.y }, color);
        DrawLineV({ start.x, end.y }, end, color);
        break;
    case WireDirection::YFirst:
        DrawLineV(start, { end.x, start.y }, color);
        DrawLineV({ end.x, start.y }, end, color);
        break;
    case WireDirection::Direct:
        DrawLineV(start, end, color);
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

void Wire::SearchConnectableTransistors(std::vector<Transistor*>* transistorArr)
{
    for (Transistor* searchStart : *transistorArr)
    {
        if (Vector2Equal(searchStart->pos, this->startPos))
        {
            this->inTransistor = searchStart;
            searchStart->outputs.push_back(this);
            break;
        }
    }
    for (Transistor* searchEnd : *transistorArr)
    {
        if (Vector2Equal(searchEnd->pos, this->endPos))
        {
            this->outTransistor = searchEnd;
            searchEnd->inputs.push_back(this);
            break;
        }
    }
}

Wire::Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction, std::vector<Transistor*>* transistorArr)
{
    startPos = _startPos;
    endPos = _endPos;
    direction = _direction;

    inTransistor = nullptr;
    outTransistor = nullptr;

    SearchConnectableTransistors(transistorArr);

    if (!inTransistor)
    {
        this->inTransistor = new Transistor(this->startPos);
        this->inTransistor->outputs.push_back(this);
        transistorArr->push_back(this->inTransistor);
    }
    if (!outTransistor)
    {
        this->outTransistor = new Transistor(this->endPos);
        this->outTransistor->inputs.push_back(this);
        transistorArr->push_back(this->outTransistor);
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

void Wire::Draw()
{
    Color color;
    if (this->active) color = RED;
    else color = WHITE;
    DrawSnappedLine(startPos, endPos, color, direction);
}

void Wire::Highlight(Color color, int width)
{
    DrawSnappedLine(startPos, endPos, color, direction, width);
}
