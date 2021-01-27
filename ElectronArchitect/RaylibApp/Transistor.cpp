#include <raylib.h>
#include <raymath.h>
#include "CustomTypes.h"
#include "Globals.h"
#include "Abstraction.h"
#include "Transistor.h"

Int2 Midpoint(Int2 start, Int2 end) { return ((start + end) / 2); }

bool Vector2Equal(const Vector2& a, const Vector2& b)
{
    return
        abs(a.x - b.x) < g_gridSize &&
        abs(a.y - b.y) < g_gridSize;
}

Vector2 Vector2Snap(const Vector2& vector)
{
    Vector2 roundOffs = { 0.5f, 0.5f };
    if (vector.x < 0.0f) roundOffs.x *= -1.0f;
    if (vector.y < 0.0f) roundOffs.y *= -1.0f;
    return Vector2{
        ((float)((int)((vector.x / g_gridSize) + roundOffs.x)) * g_gridSize),
        ((float)((int)((vector.y / g_gridSize) + roundOffs.y)) * g_gridSize)
    };
}

Int2 Transistor::JointPos(Int2 start, Int2 end, Connection::Shape direction)
{
    int xLength = Abs(end.x - start.x);
    int yLength = Abs(end.y - start.y);
    int shortestLength = (xLength < yLength ? xLength : yLength);
    Int2 shortPos;
    if (end.x > start.x) shortPos.x = shortestLength;
    else shortPos.x = -shortestLength;
    if (end.y > start.y) shortPos.y = shortestLength;
    else shortPos.y = -shortestLength;

    switch (direction)
    {
    default: case Transistor::Connection::Shape::XFirst:
        return { end.x, start.y };
        break;
    case Transistor::Connection::Shape::YFirst:
        return { start.x, end.y };
        break;
    case Transistor::Connection::Shape::DiagStart:
        return (start + shortPos);
        break;
    case Transistor::Connection::Shape::DiagEnd:
        return (end - shortPos);
        break;
    }
}

Transistor::Gate Transistor::GetGateType() const
{
    return type;
}

void Transistor::SetGateType(Gate _type)
{
    type = _type;
}

void Transistor::IncrementGate()
{
    if (OutputOnly()) type = (Gate)(((int)(type) + 1) % 2); // If the transistor has no inputs, toggle between simple & invert
    else type = (Gate)(((int)(type) + 1) % (int)Gate::Size); // Should % the number of transistor types
}

Component* Transistor::GetComponent() const
{
    return containingComponent;
}

void Transistor::SetComponent(Component* component)
{
    containingComponent = component;
}

void Transistor::SetDrawnThisFrame(bool set)
{
    if (set) flags |= DRAWN_TF; // Set true
    else flags ^= (flags & DRAWN_TF); // Set false
}

void Transistor::SetEvaluatedThisFrame(bool set)
{
    if (set) flags |= EVALUATED_TF; // Set true
    else flags ^= (flags & EVALUATED_TF); // Set false
}

void Transistor::SetBeingEvaluated(bool set)
{
    if (set) flags |= BEING_EVALUATED; // Set true
    else flags ^= (flags & BEING_EVALUATED); // Set false
}

void Transistor::SetCurrentEvaluation(bool set)
{
    if (set) flags |= EVALUATION; // Set true
    else flags ^= (flags & EVALUATION); // Set false
}

void Transistor::SetHidden(bool set)
{
    if (set) flags |= IS_HIDDEN; // Set true
    else flags ^= (flags & IS_HIDDEN); // Set false
}

void Transistor::Hide()
{
    SetHidden(true);
}

void Transistor::UnHide()
{
    SetHidden(false);
}

void Transistor::DrawSnappedLine(Vector2 start, Vector2 end, Color color, Connection::Shape direction)
{
    Vector2 jointPos;

    jointPos = (Vector2)JointPos(start, end, direction);

    DrawLineV(start, jointPos, color);
    DrawLineV(jointPos, end, color);
}
void Transistor::DrawSnappedLineEx(Vector2 start, Vector2 end, float thickness, Color color, Connection::Shape direction)
{
    Vector2 jointPos;

    jointPos = (Vector2)JointPos(start, end, direction);

    DrawLineEx(start, jointPos, thickness, color);
    DrawLineEx(jointPos, end, thickness, color);
}

void Transistor::DrawDirectionTriangle(Vector2 from, Vector2 at, Vector2 position, Color color, float scale)
{
    Vector2 directionVec = Vector2Normalize(Vector2Subtract(at, from));
    Vector2 triVerts[3];
    triVerts[0] = Vector2Scale(directionVec, 2.0f);
    triVerts[1] = Vector2Rotate(directionVec, 60.0f);
    triVerts[2] = Vector2Rotate(directionVec, -60.0f);
    for (int i = 0; i < 3; ++i) { triVerts[i] = (Vector2Subtract(Vector2Add(Vector2Scale(triVerts[i], scale), position), Vector2Scale( directionVec, scale ))); }

    DrawTriangle(triVerts[2], triVerts[1], triVerts[0], color);
}

char GateToSymbol(Transistor::Gate type)
{
    const char gateSymbol[] = "|!&^?";
    return gateSymbol[(int)type];
}
Transistor::Gate SymbolToGate(char symbol)
{
    const char gateSymbol[] = "|!&^?";
    for (char i = 0; i < sizeof(gateSymbol); ++i)
    {
        if (gateSymbol[i] == symbol) return Transistor::Gate(i);
    }
    return Transistor::Gate::Simple;
}

void Transistor::DrawTransistorIcon(Gate type, Vector2 pos, Color color, float size)
{
    switch (type)
    {
    case Transistor::Gate::Simple:
        DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos, color, size);
        break;

    case Transistor::Gate::Invert:
        DrawCircleV(pos, (size / 2.0f), color);
        break;

    case Transistor::Gate::Combine:
        DrawRectangleV({ pos.x - (size / 2.0f), pos.y - (size / 2.0f) }, { (size / 2.0f), size }, color);
        DrawCircleV(pos, (size / 2.0f), color);
        break;

    case Transistor::Gate::Choose:
        DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos, color, size);
        DrawLineEx({ pos.x - size, pos.y + size }, { pos.x - size, pos.y - size }, 2, color);
        break;

    case Transistor::Gate::Diode:
        DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos, color, size);
        DrawLineEx({ pos.x + size, pos.y + size }, { pos.x + size, pos.y - size }, 2, color);
        break;
    }
}

void Transistor::Icon(Color color, float size) const
{
    DrawTransistorIcon(type, (Vector2)pos, color, size);
}

void Transistor::Draw()
{
    if (!IsHidden() && !IsDrawnThisFrame())
    {
        bool b_startNode = (OutputOnly());
        bool b_endNode = (InputOnly());

        Color color = BLUE;
        if (b_startNode) color = GREEN;
        if (b_endNode) color = RED;

        DrawWires(); // TODO: Find a better place for this

        if (type == Gate::Diode && CurrentEvaluation()) DrawCircleV((Vector2)pos, 8.0f, ColorAlpha(WHITE, 0.5)); // Diode glow
        
        Icon(color, (((pos.x == cursorPos.x || pos.y == cursorPos.y) && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))) ? (g_gridSize * 0.6f) : (g_gridSize * 0.5f)));
        
        SetDrawnThisFrame(true);
    }
}

void Transistor::Highlight(Color color, float size) const
{
    Icon(color, size);
}

int Transistor::GetInputIndex(Transistor* connector) const
{
    if (connector == input[0].connector) return 0;
    else if (connector == input[1].connector) return 1;
    else return -1;
}

int Transistor::GetOutputIndex(Transistor* connector) const
{
    if (connector == output[0].connector) return 0;
    else if (connector == output[1].connector) return 1;
    else return -1;
}

bool Transistor::IsConnected(Transistor* connector) const
{
    if ((connector == input[0].connector) || (connector == output[0].connector) ||
        (connector == input[1].connector) || (connector == output[1].connector)) return true;
    else return false;
}

Int2 Transistor::GetPos() const
{
    return pos;
}

void Transistor::SetPos(Int2 newPos)
{
    pos = newPos;
}

void Transistor::ChangePos(Int2 amount)
{
    pos = (pos + amount);
}

bool Transistor::SolderInput(Transistor* connection, Connection::Shape direction)
{
    Connection* mine = AvailIn();
    Connection* theirs = connection->AvailOut();
    if (mine && theirs)
    {
        mine->connector = connection;
        theirs->connector = this;
        theirs->shape = direction;
        return true;
    }
    else return false;
}

bool Transistor::SolderOutput(Transistor* connection, Connection::Shape direction)
{
    Connection* mine = AvailOut();
    Connection* theirs = connection->AvailIn();
    if (mine && theirs)
    {
        mine->connector = connection;
        theirs->connector = this;
        mine->shape = direction;
        return true;
    }
    else return false;
}

bool Transistor::OutputOnly() const
{
    return (!input[0].connector && !input[1].connector);
}

bool Transistor::InputOnly() const
{
    return (!output[0].connector && !output[1].connector);
}

bool Transistor::ConnectsExternally() const
{
    return (input[0].connector || input[1].connector || output[0].connector || output[1].connector);
}

bool Transistor::IsDrawnThisFrame() const
{
    return (flags & DRAWN_TF);
}

bool Transistor::IsEvaluatedThisFrame() const
{
    return (flags & EVALUATED_TF);
}

bool Transistor::IsBeingEvaluated() const
{
    return (flags & BEING_EVALUATED);
}

bool Transistor::CurrentEvaluation() const
{
    return (flags & EVALUATION);
}

bool Transistor::IsHidden() const
{
    return (flags & IS_HIDDEN);
}

void Transistor::Evaluate()
{
    if (IsBeingEvaluated()) SetEvaluatedThisFrame(true); // Probably a loop. Just pass the existing evaluation.
    if (IsEvaluatedThisFrame()) return; // Skip the whole function

    SetBeingEvaluated(true);

    if (OutputOnly()) // 0 inputs
    {
        SetCurrentEvaluation(false);
    }
    else if (input[0].connector && input[1].connector) // 2 inputs
    {
        switch (type)
        {
        default: // OR/NOR
            SetCurrentEvaluation((input[0].connector->GetEvaluation() || input[1].connector->GetEvaluation()));
            break;
        case Gate::Combine: // AND
            SetCurrentEvaluation((input[0].connector->GetEvaluation() && input[1].connector->GetEvaluation()));
            break;
        case Gate::Choose: // XOR
            SetCurrentEvaluation(((input[0].connector->GetEvaluation()) ? (!input[1].connector->GetEvaluation()) : (input[1].connector->GetEvaluation())));
            break;
        }
    }
    else // 1 input
    {
        Transistor* evalInput = (input[0].connector ? input[0].connector : input[1].connector);
        SetCurrentEvaluation(evalInput->GetEvaluation());
    }
    if (type == Gate::Invert) SetCurrentEvaluation(!CurrentEvaluation());

    SetEvaluatedThisFrame(true);
    SetBeingEvaluated(false);
}

Transistor::Connection* Transistor::AvailIn()
{
    if (!input[0].connector) return &(input[0]);
    else if (!input[1].connector) return &(input[1]);
    else return nullptr;
}

Transistor::Connection* Transistor::AvailOut()
{
    if (!output[0].connector) return &(output[0]);
    else if (!output[1].connector) return &(output[1]);
    else return nullptr;
}

bool Transistor::GetEvaluation()
{
    Evaluate();
    return CurrentEvaluation();
}

void Transistor::DrawWires()
{
    Color color = WHITE;
    float thickness = 0.5f;
    if (GetEvaluation())
    {
        color = RED;
        thickness = 1;
    }
    if (output[0].connector && !output[0].connector->IsHidden()) DrawSnappedLine((Vector2)GetPos(), (Vector2)output[0].connector->GetPos(), color, output[0].shape);
    if (output[1].connector && !output[1].connector->IsHidden()) DrawSnappedLine((Vector2)GetPos(), (Vector2)output[1].connector->GetPos(), color, output[1].shape);
}

// Called at the end of every frame
void Transistor::FrameReset()
{
    SetDrawnThisFrame(false);
    SetEvaluatedThisFrame(false);
}

void Transistor::ClearReferences()
{
    // Replace references to myself with nullptrs
    if (input[0].connector)
    {
        int myIndex = input[0].connector->GetOutputIndex(this);
        if (myIndex != -1) input[0].connector->output[myIndex] = nullptr;
    }
    if (input[1].connector)
    {
        int myIndex = input[1].connector->GetOutputIndex(this);
        if (myIndex != -1) input[1].connector->output[myIndex] = nullptr;
    }
    if (output[0].connector)
    {
        int myIndex = output[0].connector->GetInputIndex(this);
        if (myIndex != -1) output[0].connector->input[myIndex] = nullptr;
    }
    if (output[1].connector)
    {
        int myIndex = output[1].connector->GetInputIndex(this);
        if (myIndex != -1) output[1].connector->input[myIndex] = nullptr;
    }
    Erase(s_allTransistors, this); // Stop tryina draw the damn thing.
}

void Transistor::Connection::IncrementShape()
{
    shape = (Shape)(((int)shape + 1) % (int)Shape::Size);
}

Transistor::Connection::Shape Transistor::Connection::NextShape(Shape input)
{
    return Shape((int)input + 1);
}

std::vector<Transistor*> Transistor::s_allTransistors;
