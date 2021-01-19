#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

Vector2 operator*(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, float scale);
Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 Vector2Midpoint(Vector2 start, Vector2 end);

void DrawDirectionTriangle(Vector2 from, Vector2 at, Vector2 position, Color color);

struct Wire;

struct Transistor
{
    enum class Type
    {
        Simple = 0, // Same as "OR"
        Invert,     // Same as "NOR"
        Combine,    // Same as "AND"
        Choose,     // Same as "XOR"

        Size // Used for %
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
