#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Wire;

struct Transistor
{
    enum class Type
    {
        Simple = 0, // Same as "OR"
        Invert,     // Same as "NOR"
    };

    Transistor() : type{ Type::Simple }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false } { };
    Transistor(Type _type) : type{ _type }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }  { };
    Transistor(Vector2 _pos) : type{ Type::Simple }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }  { };
    Transistor(Type _type, Vector2 _pos) : type{ _type }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }  { };

    std::vector<Wire*> inputs;
    std::vector<Wire*> outputs;
    Vector2 pos;
    Type type;
    bool b_drawnThisFrame, b_evaluatedThisFrame;

    bool HasConnections();

    void Evaluate();

    void Draw();

    void FrameReset();
};
