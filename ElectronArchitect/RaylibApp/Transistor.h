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

    Transistor() : type{ Type::Simple }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false } { };
    Transistor(Type _type) : type{ _type }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }  { };
    Transistor(Vector2 _pos) : type{ Type::Simple }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }  { };
    Transistor(Type _type, Vector2 _pos) : type{ _type }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }  { };

    std::vector<Wire*> inputs;
    std::vector<Wire*> outputs;
    Vector2 pos;
    Type type;
    bool b_drawnThisFrame, b_evaluatedThisFrame, b_beingEvaluated;

    bool OutputOnly() const;    // Is the start of a tree
    bool InputOnly() const;     //Is the end of a tree
    bool ConnectsExternally() const;

    void Evaluate();

    void Draw();
    void Highlight(Color color, int size) const;

    void FrameReset();
};
