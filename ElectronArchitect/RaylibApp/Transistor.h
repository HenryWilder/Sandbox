#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

Vector2 operator*(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, float scale);
Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 Vector2Midpoint(Vector2 start, Vector2 end);
bool Vector2Equal(const Vector2& a, const Vector2& b);
Vector2 Vector2Snap(const Vector2& vector);

class Component;

class Transistor
{
public:

    enum class Gate : char
    {
        Simple = 0, // Same as "OR"
        Invert,     // Same as "NOR"
        Combine,    // Same as "AND"
        Choose,     // Same as "XOR"
        Diode,      // LED

        Size // Used for %
    };

    struct Connection // Connections will never have "new" or "delete" called, so there will never be a dangling reference.
    {
        enum class Shape : char
        {
            XFirst = 0,
            YFirst,
            DiagStart,
            DiagEnd,

            Size // Total directions (for %)
        };
        Connection() : shape{ Shape::XFirst }, connector{ nullptr } {};
        Connection(Transistor* _connector) : shape{ Shape::XFirst }, connector{ _connector } {};
        Connection(Transistor* _connector, Shape _shape) : shape{ (_shape == Shape::Size ? Shape::XFirst : _shape) }, connector{ _connector } {};

        Shape shape;
        Transistor* connector;

        void IncrementShape();
        static Shape NextShape(Shape);
    };

    Transistor() : type{ Gate::Simple }, input{ {},{} }, output{ {},{} }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Gate _type) : input{ {},{} }, output{ {},{} }, type{ _type }, pos{ }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Vector2 _pos) : input{ {},{} }, output{ {},{} }, type{ Gate::Simple }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Gate _type, Vector2 _pos) : type{ _type }, input{ {},{} }, output{ {},{} }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Transistor* _input) : type{  }, input{ { _input },{} }, output{ {},{} }, pos{  }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); SolderInput(_input, Connection::Shape::XFirst); };
    Transistor(Transistor* _input, Vector2 _pos) : type{  }, input{ {},{} }, output{ {},{} }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); SolderInput(_input, Connection::Shape::XFirst); };
    Transistor(Transistor* _input, Vector2 _pos, Connection::Shape _shape) : type{  }, input{ {},{} }, output{ {},{} }, pos{ _pos }, b_drawnThisFrame{ false }, b_evaluatedThisFrame{ false }, b_beingEvaluated{ false }, evaluation{ false }, b_hidden{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); SolderInput(_input, _shape); };

    ~Transistor();

    static std::vector<Transistor*> s_allTransistors;

private:

    Connection input[2];
    Connection output[2];
    Vector2 pos;
    Gate type;
    bool b_drawnThisFrame, b_evaluatedThisFrame, b_beingEvaluated, evaluation, b_hidden;

    Component* containingComponent;

    void Evaluate();
    Connection* AvailIn();
    Connection* AvailOut();

    static Vector2 JointPos(Vector2 start, Vector2 end, Connection::Shape direction);

public:

    Gate GetGateType() const;
    void SetGateType(Gate _type);
    void IncrementGate();
    Component* GetComponent() const;

    bool IsHidden() const;
    void Hide();
    void UnHide();
    void SetHidden(bool);

    int GetInputIndex(Transistor* connector) const;
    int GetOutputIndex(Transistor* connector) const;
    bool IsConnected(Transistor* connector) const;

    Vector2 GetPos() const;
    void SetPos(Vector2 newPos);
    void ChangePos(Vector2 amount);

    bool SolderInput(Transistor* connection, Connection::Shape direction);
    bool SolderOutput(Transistor* connection, Connection::Shape direction);

    bool OutputOnly() const;    // Is the start of a tree
    bool InputOnly() const;     //Is the end of a tree
    bool ConnectsExternally() const;

    bool GetEvaluation();

    static void DrawSnappedLine(Vector2 start, Vector2 end, Color color, Connection::Shape direction);
    static void DrawSnappedLineEx(Vector2 start, Vector2 end, float thickness, Color color, Connection::Shape direction);

    static void DrawDirectionTriangle(Vector2 from, Vector2 at, Vector2 position, Color color, float scale);
    static void DrawTransistorIcon(Gate type, Vector2 pos, Color color, float size);

    void DrawWires(); // Draws the output wire(s)

    void Icon(Color color, float size) const;
    void Draw(); // Not const because it modifies b_drawnThisFrame
    void Highlight(Color color, float size) const;

    void FrameReset();

    friend void Save(const std::vector<Transistor*>&);
    friend void Load(std::vector<Transistor*>&);

    friend class Component;
    friend Component* MakeAbstract(std::vector<Transistor*>& selection, Vector2 position, float g_gridSize);
};

extern std::vector<Transistor*> s_allTransistors;

char GateToSymbol(Transistor::Gate type);
Transistor::Gate SymbolToGate(char symbol);
