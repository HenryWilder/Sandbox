#include "Wire.h"
#include "Transistor.h"

Vector2 JointPos(Vector2 start, Vector2 end, WireDirection direction)
{
    float xLength = abs(end.x - start.x);
    float yLength = abs(end.y - start.y);
    float shortestLength = (xLength < yLength ? xLength : yLength);
    Vector2 shortPos;
    if (end.x > start.x) shortPos.x = shortestLength;
    else shortPos.x = -1.0f * shortestLength;
    if (end.y > start.y) shortPos.y = shortestLength;
    else shortPos.y = -1.0f * shortestLength;

    switch (direction)
    {
    default: case WireDirection::XFirst:
        return { end.x, start.y };
        break;
    case WireDirection::YFirst:
        return { start.x, end.y };
        break;
    case WireDirection::DiagStart:
        return (start + shortPos);
        break;
    case WireDirection::DiagEnd:
        return (end - shortPos);
        break;
    }
}

Vector2 Wire::GetJointPos() const
{
    return JointPos(GetStartPos(), GetEndPos(), direction);
}

void DrawSnappedLine(Vector2 start, Vector2 end, Color color, WireDirection direction)
{
    Vector2 jointPos;

    jointPos = JointPos(start, end, direction);

    DrawLineV(start, jointPos, color);
    DrawLineV(jointPos, end, color);
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

bool Wire::IsPointOnLine(Vector2 point) const
{
    Vector2 joint = GetJointPos();

    float segmentLen[2] = {
        Vector2Distance(GetStartPos(), joint),
        Vector2Distance(joint, GetEndPos())
    };
    float pointDist[3] = {
        Vector2Distance(point, GetStartPos()),
        Vector2Distance(point, joint),
        Vector2Distance(point, GetEndPos())
    };
    return (((pointDist[0] + pointDist[1]) <= segmentLen[0]) || ((pointDist[1] + pointDist[2]) <= segmentLen[1]));
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
