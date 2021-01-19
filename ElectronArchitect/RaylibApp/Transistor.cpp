#include "Transistor.h"
#include "Wire.h"

Vector2 operator*(Vector2 a, Vector2 b) { return { a.x * b.x, a.y * b.y }; }
Vector2 operator*(Vector2 a, float scale) { return { a.x * scale, a.y * scale }; }
Vector2 operator+(Vector2 a, Vector2 b) { return { a.x + b.x, a.y + b.y }; }
Vector2 operator-(Vector2 a, Vector2 b) { return { a.x - b.x, a.y - b.y }; }

Vector2 Vector2Midpoint(Vector2 start, Vector2 end) { return ((start + end) * 0.5f); }

void DrawDirectionTriangle(Vector2 from, Vector2 at, Vector2 position, Color color)
{
    Vector2 directionVec = Vector2Normalize((at - from));
    Vector2 triVerts[3];
    triVerts[0] = directionVec * 2.0f;
    triVerts[1] = Vector2Rotate(directionVec, 60.0f);
    triVerts[2] = Vector2Rotate(directionVec, -60.0f);
    for (int i = 0; i < 3; ++i) { triVerts[i] = ((triVerts[i] * 6.0f) + position); }

    DrawTriangle(triVerts[2], triVerts[1], triVerts[0], color);
}

void Transistor::Draw()
{
    if (!b_drawnThisFrame)
    {
        bool b_startNode = (inputs.size() == 0);
        bool b_endNode = (outputs.size() == 0);

        Color color = GREEN;
        if (b_startNode) color = YELLOW;
        if (b_endNode) color = RED;

        //color = ColorAlpha(color, 0.5);

        Vector2 joint = (b_startNode ? pos : inputs[0]->GetJointPos());

        switch (type)
        {
        case Type::Simple:
            DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos - Vector2{ 6,0 }, color);
            break;

        case Type::Invert:
            DrawCircleV(pos, 2.0f, color);
            break;

        case Type::Combine:
            DrawRectangle(pos.x - 4, pos.y - 3, 6, 6, color);
            DrawCircle(pos.x + 2, pos.y, 3.0f, color);
            break;

        case Type::Choose:
            DrawDirectionTriangle({ 0,0 }, { 1,0 }, pos - Vector2{ 6,0 }, color);
            DrawRectangle(pos.x - 7, pos.y - 4, 2, 8, color);
            break;
        }
        b_drawnThisFrame = true;
    }
}

void Transistor::Highlight(Color color, int size) const
{
    switch (type)
    {
    case Type::Simple:
        DrawRectangleV({ pos.x - ((float)size / 2.0f), pos.y - ((float)size / 2.0f) }, { (float)size, (float)size }, color);
        break;

    case Type::Invert:
        DrawCircleV(pos, ((float)size / 2.0f), color);
        break;

    case Type::Combine:
        DrawRectangleV({ pos.x - 2.0f, pos.y - 2.0f }, { 4.0f, 4.0f }, color);
        DrawCircleV(pos, 3.0f, color);
        break;

    case Type::Choose:
        DrawRectangleV({ pos.x - ((float)size / 2.0f), pos.y - ((float)size / 2.0f) }, { (float)size, (float)size }, color);
        break;
    }
}

bool Transistor::OutputOnly() const
{
    return (inputs.size() == 0);
}

bool Transistor::InputOnly() const
{
    return (outputs.size() == 0);
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
            bool evaluation;

            switch (type)
            {
            default: case Type::Simple: case Type::Invert: // OR/NOR

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

                if (type == Type::Invert) evaluation = !evaluation;
                break;

            case Type::Combine: // AND

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

            case Type::Choose: // XOR

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
