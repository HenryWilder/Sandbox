#include "Wire.h"
#include "Globals.h"
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
void DrawSnappedLineEx(Vector2 start, Vector2 end, float thickness, Color color, WireDirection direction)
{
    Vector2 jointPos;

    jointPos = JointPos(start, end, direction);

    DrawLineEx(start, jointPos, thickness, color);
    DrawLineEx(jointPos, end, thickness, color);
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

void Wire::SearchConnectableTransistors(Vector2 startPos, Vector2 endPos)
{
    for (Transistor* searchStart : Transistor::s_allTransistors)
    {
        if (Vector2Equal(searchStart->pos, startPos) && (!searchStart->b_hidden))
        {
            inTransistor = searchStart;
            searchStart->outputs.push_back(this);
            break;
        }
    }
    for (Transistor* searchEnd : Transistor::s_allTransistors)
    {
        if (Vector2Equal(searchEnd->pos, endPos) && (!searchEnd->b_hidden))
        {
            outTransistor = searchEnd;
            searchEnd->inputs.push_back(this);
            break;
        }
    }
}

float Wire::GetLength() const
{
    return abs(Vector2Length(GetJointPos() - GetStartPos())) + abs(Vector2Length(GetEndPos() + GetJointPos()));
}

void Wire::ClearReferences()
{
    if (inTransistor) Erase(inTransistor->outputs, this);
    if (outTransistor) Erase(outTransistor->inputs, this);
    Erase(Wire::allWires, this);
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

Wire::Wire(Vector2 _startPos, Vector2 _endPos, WireDirection _direction)
{
    direction = _direction;

    inTransistor = nullptr;
    outTransistor = nullptr;

    SearchConnectableTransistors(_startPos, _endPos);

    if (!inTransistor)
    {
        inTransistor = new Transistor(_startPos);
        inTransistor->outputs.push_back(this);
        Transistor::s_allTransistors.push_back(inTransistor);
    }
    if (!outTransistor)
    {
        outTransistor = new Transistor(_endPos);
        outTransistor->inputs.push_back(this);
        Transistor::s_allTransistors.push_back(outTransistor);
    }

    active = false;
    hidden = false;
}

void Wire::Draw() const
{
    if (!hidden)
    {
        Color color;
        if (active) color = RED;
        else color = WHITE;
        DrawSnappedLine(inTransistor->pos, outTransistor->pos, color, direction);
    }
}

void Wire::Highlight(Color color, float width) const
{
    if (!hidden)
    {
        DrawSnappedLineEx(inTransistor->pos, outTransistor->pos, width, color, direction);
    }
}

std::vector<Wire*> Wire::allWires;

//Wire::~Wire()
//{
//    if (inTransistor) Erase(inTransistor->outputs, this);
//    if (outTransistor) Erase(outTransistor->inputs, this);
//}
