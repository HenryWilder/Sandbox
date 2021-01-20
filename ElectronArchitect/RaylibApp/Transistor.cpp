#include "Transistor.h"
#include "Globals.h"
#include "Wire.h"
#include "Abstraction.h"

Vector2 operator*(Vector2 a, Vector2 b) { return { a.x * b.x, a.y * b.y }; }
Vector2 operator*(Vector2 a, float scale) { return { a.x * scale, a.y * scale }; }
Vector2 operator+(Vector2 a, Vector2 b) { return { a.x + b.x, a.y + b.y }; }
Vector2 operator-(Vector2 a, Vector2 b) { return { a.x - b.x, a.y - b.y }; }

Vector2 Vector2Midpoint(Vector2 start, Vector2 end) { return ((start + end) * 0.5f); }

void DrawDirectionTriangle(Vector2 from, Vector2 at, Vector2 position, Color color, float scale)
{
    Vector2 directionVec = Vector2Normalize((at - from));
    Vector2 triVerts[3];
    triVerts[0] = directionVec * 2.0f;
    triVerts[1] = Vector2Rotate(directionVec, 60.0f);
    triVerts[2] = Vector2Rotate(directionVec, -60.0f);
    for (int i = 0; i < 3; ++i) { triVerts[i] = ((triVerts[i] * scale) + position - Vector2{ directionVec * scale }); }

    DrawTriangle(triVerts[2], triVerts[1], triVerts[0], color);
}

char GateToSymbol(TransistorType type)
{
    const char gateSymbol[] = "|!&^?";
    return gateSymbol[(int)type];
}
TransistorType SymbolToGate(char symbol)
{
    const char gateSymbol[] = "|!&^?";
    for (char i = 0; i < sizeof(gateSymbol); ++i)
    {
        if (gateSymbol[i] == symbol) return TransistorType(i);
    }
    return TransistorType::Simple;
}

void DrawTransistorIcon(TransistorType type, Vector2 pos, Color color, float size)
{
    switch (type)
    {
    case TransistorType::Simple:
        DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos, color, size);
        break;

    case TransistorType::Invert:
        DrawCircleV(pos, (size / 2.0f), color);
        break;

    case TransistorType::Combine:
        DrawRectangleV({ pos.x - (size / 2.0f), pos.y - (size / 2.0f) }, { (size / 2.0f), size }, color);
        DrawCircleV(pos, (size / 2.0f), color);
        break;

    case TransistorType::Choose:
        DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos, color, size);
        DrawLineEx({ pos.x - size, pos.y + size }, { pos.x - size, pos.y - size }, 2, color);
        break;

    case TransistorType::Diode:
        DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos, color, size);
        DrawLineEx({ pos.x + size, pos.y + size }, { pos.x + size, pos.y - size }, 2, color);
        break;
    }
}

void Transistor::Icon(Color color, float size) const
{
    DrawTransistorIcon(type, pos, color, size);
}

void Transistor::Draw()
{
    if (!b_drawnThisFrame)
    {
        bool b_startNode = (inputs.size() == 0);
        bool b_endNode = (outputs.size() == 0);

        Color color = BLUE;
        if (b_startNode) color = GREEN;
        if (b_endNode) color = RED;

        //color = ColorAlpha(color, 0.5);

        Vector2 joint = (b_startNode ? pos : inputs[0]->GetJointPos());

        // Diode glow
        if (type == TransistorType::Diode && evaluation) DrawCircleV(pos, 8.0f, ColorAlpha(WHITE, 0.5));
        
        Icon(color, (((pos.x == cursorPos.x || pos.y == cursorPos.y) && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))) ? (g_gridSize * 0.6f) : (g_gridSize * 0.5f)));
        
        b_drawnThisFrame = true;
    }
}

void Transistor::Highlight(Color color, float size) const
{
    Icon(color, size);
}

bool Transistor::OutputOnly() const
{
    return (inputs.empty());
}

bool Transistor::InputOnly() const
{
    return (outputs.empty());
}

bool Transistor::ConnectsExternally() const
{
    return ((outputs.size() + inputs.size()) > 1);
}

void Transistor::Evaluate()
{
    if (!b_evaluatedThisFrame)
    {
        if (!b_beingEvaluated)
        {
            b_beingEvaluated = true;

            switch (type)
            {
            default: case TransistorType::Simple: case TransistorType::Invert: // OR/NOR

                evaluation = false;
                for (Wire* inputWire : inputs)
                {
                    if (!inputWire->inTransistor->b_evaluatedThisFrame) inputWire->inTransistor->Evaluate();

                    if (inputWire->active)
                    {
                        evaluation = true;
                        break;
                    }
                }

                if (type == TransistorType::Invert) evaluation = !evaluation;
                break;

            case TransistorType::Combine: // AND

                evaluation = true;
                for (Wire* inputWire : inputs)
                {
                    if (!inputWire->inTransistor->b_evaluatedThisFrame) inputWire->inTransistor->Evaluate();

                    if (!inputWire->active)
                    {
                        evaluation = false;
                        break;
                    }
                }
                break;

            case TransistorType::Choose: // XOR

                evaluation = false;
                for (Wire* inputWire : inputs)
                {
                    if (!inputWire->inTransistor->b_evaluatedThisFrame) inputWire->inTransistor->Evaluate();

                    if (inputWire->active)
                    {
                        if (evaluation)
                        {
                            evaluation = false;
                            break;
                        }
                        else evaluation = true;
                    }
                }
                break;
            }

            b_evaluatedThisFrame = true;

            for (Wire* outputWire : outputs)
            {
                outputWire->active = evaluation;
                outputWire->outTransistor->Evaluate();
            }

            b_beingEvaluated = false;
        }
    }
}

// Called at the end of every frame
void Transistor::FrameReset()
{
    b_drawnThisFrame = false;
    b_evaluatedThisFrame = false;
}

std::vector<Transistor*> Transistor::allTransistors;
