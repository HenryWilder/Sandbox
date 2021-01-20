#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

Vector2 operator*(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, float scale);
Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 Vector2Midpoint(Vector2 start, Vector2 end);

void DrawDirectionTriangle(Vector2 from, Vector2 at, Vector2 position, Color color, float scale);

struct Wire;

enum class TransistorType : char
{
    Simple = 0, // Same as "OR"
    Invert,     // Same as "NOR"
    Combine,    // Same as "AND"
    Choose,     // Same as "XOR"
    Diode,      // LED

    Size // Used for %
};
char GateToSymbol(TransistorType type);
TransistorType SymbolToGate(char symbol);

void DrawTransistorIcon(TransistorType type, Vector2 pos, Color color, int size);

struct Transistor
{
    Transistor() : type{ TransistorType::Simple }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }{ };
    Transistor(TransistorType _type) : type{ _type }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }{ };
    Transistor(Vector2 _pos) : type{ TransistorType::Simple }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }{ };
    Transistor(TransistorType _type, Vector2 _pos) : type{ _type }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }{ };

    static std::vector<Transistor*> allTransistors;
    std::vector<Wire*> inputs;
    std::vector<Wire*> outputs;
    Vector2 pos;
    TransistorType type;
    bool b_drawnThisFrame, b_evaluatedThisFrame, b_beingEvaluated, evaluation;

    bool OutputOnly() const;    // Is the start of a tree
    bool InputOnly() const;     //Is the end of a tree
    bool ConnectsExternally() const;

    void Evaluate();

    void Icon(Color color, float size) const;
    void Draw();
    void Highlight(Color color, float size) const;

    void FrameReset();
};

extern std::vector<Transistor*> allTransistors;
