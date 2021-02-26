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

bool Transistor::ANY(std::vector<Transistor*>& in)
{
    for (Transistor* elem : in) {
        if (elem->GetEvaluation()) return true;
    }
    return false;
}

bool Transistor::NOR(std::vector<Transistor*>& in)
{
    return !ANY(in);
}

bool Transistor::AND(std::vector<Transistor*>& in)
{
    for (Transistor* elem : in) {
        if (!elem->GetEvaluation()) return false;
    }
    return true;
}

bool Transistor::XOR(std::vector<Transistor*>& in)
{
    bool b;
    for (Transistor* elem : in) {
        if (elem->GetEvaluation()) {
            if (!b) b = true;
            else return false;
        }
    }
    return b;
}

Transistor* Transistor::Get(ID_t id)
{
    auto it = s_transistorMap.find(id);
    if (s_transistorMap.valid(it)) return it->second;
    else return nullptr;
}

ID_t Transistor::Create(Int2 _pos = Int2(), Gate _type = Gate::Simple)
{
    Transistor* t = new Transistor(_pos, _type);
    ID_t id = s_transistorMap.push(t);
    t->myID = id;
    return id;
}

void Transistor::Destroy(const ID_t id)
{
    auto it = s_transistorMap.find(id);
    if (s_transistorMap.valid(it)) {
        delete it->second;
        s_transistorMap.erase(id);
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

Transistor::Flag Transistor::operator|(Flag a, Flag b)
{
    return Flag((unsigned char)a | (unsigned char)b);
}

unsigned char Transistor::TestFlags(unsigned char flags) const
{
    return flags & flags;
}
unsigned char Transistor::TestFlags(Flag flags) const
{
    return TestFlags((unsigned char)flags);
}
void Transistor::SetFlags(unsigned char flags, unsigned char set)
{
    flags = ((flags & ~flags) | (set & flags));
}

void Transistor::SetFlags(Flag flags, unsigned char set)
{
    SetFlags((unsigned char)flags, set);
}

void Transistor::SetFlags(unsigned char flags, bool set)
{
    SetFlags(flags, (set ? (unsigned char)0b11111111 : (unsigned char)0b00000000));
}

void Transistor::SetFlags(Flag flags, bool set)
{
    SetFlags((unsigned char)flags, set);
}


void Transistor::Hide()
{
    SetFlags(Flag::IS_HIDDEN, true);
}

void Transistor::UnHide()
{
    SetFlags(Flag::IS_HIDDEN, false);
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
    if (!TestFlags(Flag::IS_HIDDEN | Flag::DRAWN_TF))
    {
        bool b_startNode = (OutputOnly());
        bool b_endNode = (InputOnly());

        Color color = BLUE;
        if (b_startNode) color = GREEN;
        if (b_endNode) color = RED;

        DrawWires(); // TODO: Find a better place for this

        if (type == Gate::Diode && GetEvaluation()) DrawCircleV((Vector2)pos, 8.0f, ColorAlpha(WHITE, 0.5)); // Diode glow
        
        Icon(color,
             (((pos.x == cursorPos.x || pos.y == cursorPos.y) && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))) ?
              (g_gridSize * 0.6f) :
              (g_gridSize * 0.5f))
        );
        
        SetFlags(Flag::DRAWN_TF, true);
    }
}

void Transistor::Highlight(Color color, float size) const
{
    Icon(color, size);
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

bool Transistor::SolderInput(ID_t connection, Connection::Shape direction)
{
    auto it = s_transistorMap.find(connection);
    if (s_transistorMap.valid(it))
    {
        prev.emplace_back(Connection(connection, direction));
        it->second->next.emplace_back(Connection(myID, direction));
        return true;
    }
    else return false;
}
bool Transistor::SolderOutput(ID_t connection, Connection::Shape direction)
{
    auto it = s_transistorMap.find(connection);
    if (s_transistorMap.valid(it))
    {
        next.emplace_back(Connection(connection, direction));
        it->second->prev.emplace_back(Connection(myID, direction));
        return true;
    }
    else return false;
}
bool Transistor::DesolderInput(int which)
{

}
bool Transistor::DesolderInput(ID_t what)
{

}
bool Transistor::DesolderOutput(int which)
{

}
bool Transistor::DesolderOutput(ID_t what)
{

}

bool Transistor::OutputOnly() const
{
    return (prev.empty() && !next.empty());
}

bool Transistor::InputOnly() const
{
    return (!prev.empty() && next.empty());
}

bool Transistor::ConnectsExternally() const
{
    return (!prev.empty() || !next.empty());
}

bool Transistor::IsDisconnected() const
{
    return (prev.empty() && next.empty());
}

std::vector<Transistor*> Transistor::Validate(std::vector<Connection>& arr)
{
    std::vector<Transistor*> valid;
    valid.reserve(arr.size()); // We can be certain there will be at MOST prev.size() valid inputs

    std::vector<unsigned int> invalidConnectors;
    invalidConnectors.reserve(arr.size()); // We can be certain there will be at MOST prev.size() invalids

    for (int i = 0; i < arr.size(); ++i) {
        auto it = s_transistorMap.find(arr[i].connector);
        if (s_transistorMap.valid(it)) valid.push_back(it->second);
        else invalidConnectors.push_back(i);
    }
    EraseMulti(arr, invalidConnectors);

    return valid;
}

ID_t Transistor::GetID()
{
    return myID;
}

void Transistor::Evaluate()
{
    if (TestFlags(Flag::BEING_EVALUATED)) SetFlags(Flag::EVALUATED_TF, true); // Probably a loop. Just pass the existing evaluation.
    if (TestFlags(Flag::EVALUATED_TF)) return; // Skip the whole function

    SetFlags(Flag::BEING_EVALUATED, true);

    std::vector<Transistor*> validInputs = Validate(prev);

    if (OutputOnly()) { // 0 inputs
        if (GetGateType() == Gate::Invert) SetFlags(Flag::EVALUATION, true);
        else SetFlags(Flag::EVALUATION, false);
    }
    else { // Non-0 inputs
        switch (type)
        {
        default: // OR
            SetFlags(Flag::EVALUATION, ANY(validInputs));
            break;
        case Gate::Invert: // NOR
            SetFlags(Flag::EVALUATION, NOR(validInputs));
            break;
        case Gate::Combine: // AND
            SetFlags(Flag::EVALUATION, AND(validInputs));
            break;
        case Gate::Choose: // XOR
            SetFlags(Flag::EVALUATION, XOR(validInputs));
            break;
        }
    }

    SetFlags(Flag::EVALUATED_TF | Flag::BEING_EVALUATED, Flag::EVALUATED_TF);
}

bool Transistor::GetEvaluation()
{
    if (!Flag::EVALUATED_TF) Evaluate();
    return TestFlags(Flag::EVALUATION);
}

void Transistor::DrawWires()
{
    Color color;
    float thickness;

    if (GetEvaluation()) {
        color = RED;
        thickness = 1;
    }
    else {
        color = WHITE;
        thickness = 0.5f;
    }

    Validate(next);

    for (Connection wire : next)
    {
        Transistor* transistor = s_transistorMap.find(wire.connector)->second;

        if (!transistor->TestFlags(Flag::IS_HIDDEN)) {
            DrawSnappedLine( (Vector2)GetPos(), (Vector2)transistor->GetPos(), color, wire.shape );
        }
    }
}

// Called at the end of every frame
void Transistor::FrameReset() { SetFlags(Flag::DRAWN_TF | Flag::EVALUATED_TF, false); }

void Transistor::Connection::IncrementShape() { shape = (Shape)(((int)shape + 1) % (int)Shape::Size); }

Transistor::Connection::Shape Transistor::Connection::NextShape(Shape input) { return Shape((int)input + 1); }

ID_t::operator uint32_t() { return id; }
Transistor* ID_t::operator->() { return Transistor::s_transistorMap.find(*this)->second; }
Transistor* ID_t::operator*() { return Transistor::s_transistorMap.find(*this)->second; }
ID_t& ID_t::operator=(ID_t b) { id = b; return *this; }
ID_t::operator bool() { return Transistor::s_transistorMap.valid(*this); }

bool ID_t::operator==(ID_t b) { return id == b.id; }

bool ID_t::operator!=(ID_t b) { return id != b.id; }
