#include "Transistor.h"
#include "Wire.h"

void Transistor::Draw()
{
    if (!b_drawnThisFrame)
    {
        bool b_startNode = (inputs.size() == 0);
        bool b_endNode = (outputs.size() == 0);

        Color color = GREEN;
        if (b_startNode) color = YELLOW;
        if (b_endNode) color = RED;

        switch (type)
        {
        case Type::Simple:
            DrawCircleV(pos, 3.0f, ColorAlpha(color, 0.5));
            break;

        case Type::Invert:
            DrawRectangleV({ pos.x - 2.0f, pos.y - 2.0f }, { 4.0f, 4.0f }, ColorAlpha(color, 0.5));
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
        DrawCircleV(pos, ((float)size / 2.0f), color);
        break;

    case Type::Invert:
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
            bool evaluation = false;

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
