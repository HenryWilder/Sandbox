#include "Transistor.h"
#include "Wire.h"

void Transistor::Draw()
{
    if (!b_drawnThisFrame)
    {
        switch (type)
        {
        case Type::Simple:
            DrawCircleV(pos, 2.0f * inputs.size(), ColorAlpha(RED, 0.5));
            DrawCircleV(pos, 2.0f * outputs.size(), ColorAlpha(GREEN, 0.5));
            break;

        case Type::Invert:
            DrawRectangleV({ pos.x - (2.0f * (float)inputs.size()), pos.y - (2.0f * (float)inputs.size()) }, { 4.0f * inputs.size(), 4.0f * inputs.size() }, ColorAlpha(RED, 0.5));
            DrawRectangleV({ pos.x - (2.0f * (float)outputs.size()), pos.y - (2.0f * (float)outputs.size()) }, { 4.0f * outputs.size(), 4.0f * outputs.size() }, ColorAlpha(GREEN, 0.5));
            break;
        }
        b_drawnThisFrame = true;
    }
}

bool Transistor::HasConnections()
{
    return ((inputs.size() > 1) || (outputs.size() > 1));
}

void Transistor::Evaluate()
{
    if (!b_evaluatedThisFrame)
    {
        bool evaluation = false;

        switch (type)
        {
        case Type::Simple: // If any is true, evaluate true.
            evaluation = false;
            for (Wire* inputWire : inputs)
            {
                if (inputWire->active)
                {
                    evaluation = true;
                    break;
                }
            }
            break;

        case Type::Invert: // If any is true, evaluate false.
            evaluation = true;
            for (Wire* inputWire : inputs)
            {
                if (inputWire->active)
                {
                    evaluation = false;
                    break;
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
    }
}

// Called at the end of every frame
void Transistor::FrameReset()
{
    b_drawnThisFrame = false;
    b_evaluatedThisFrame = false;
}
