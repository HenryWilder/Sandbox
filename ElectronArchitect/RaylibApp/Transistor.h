#pragma once
#include <vector>

struct Color;
struct Vector2;
struct Int2;

Int2 Midpoint(Int2 start, Int2 end);
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

    Transistor() : input{ {},{} }, output{ {},{} }, pos{ }, type{ Gate::Simple }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Gate _type) : input{ {},{} }, output{ {},{} }, type{ _type }, pos{ }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Int2 _pos) : input{ {},{} }, output{ {},{} }, type{ Gate::Simple }, pos{ _pos }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Gate _type, Int2 _pos) : input{ {},{} }, output{ {},{} }, pos{ _pos }, type{ _type }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); };
    Transistor(Transistor* _input) : input{ { _input },{} }, output{ {},{} }, pos{  }, type{  }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); SolderInput(_input, Connection::Shape::XFirst); };
    Transistor(Transistor* _input, Int2 _pos) : input{ {},{} }, output{ {},{} }, pos{ _pos }, type{  }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); SolderInput(_input, Connection::Shape::XFirst); };
    Transistor(Transistor* _input, Int2 _pos, Connection::Shape _shape) : input{ {},{} }, output{ {},{} }, pos{ _pos }, type{  }, flags{ false }, containingComponent{ nullptr }{ s_allTransistors.push_back(this); SolderInput(_input, _shape); };
    ~Transistor() { printf("Transistor deleted! %zu transistors remain.\n", s_allTransistors.size()); }

    static std::vector<Transistor*> s_allTransistors;

    Connection input[2];
    Connection output[2];

private:
    Int2 pos;
    Gate type;
    enum Flag : unsigned char {
        DRAWN_TF =          0b00000001,
        EVALUATED_TF =      0b00000010,
        BEING_EVALUATED =   0b00000100,
        EVALUATION =        0b00001000,
        IS_HIDDEN =         0b00010000,
    };
    char flags;

public:
    bool IsDrawnThisFrame() const;
    bool IsEvaluatedThisFrame() const;
    bool IsBeingEvaluated() const;
    bool CurrentEvaluation() const;
    bool IsHidden() const;

    void SetDrawnThisFrame(bool set);
    void SetEvaluatedThisFrame(bool set);
    void SetBeingEvaluated(bool set);
    void SetCurrentEvaluation(bool set);
    void SetHidden(bool set);

private:
    Component* containingComponent;

    void Evaluate();
    Connection* AvailIn();
    Connection* AvailOut();

    static Int2 JointPos(Int2 start, Int2 end, Connection::Shape direction);

public:

    Gate GetGateType() const;
    void SetGateType(Gate _type);
    void IncrementGate();
    Component* GetComponent() const;
    void SetComponent(Component* component);
    
    void Hide();
    void UnHide();

    int GetInputIndex(Transistor* connector) const;
    int GetOutputIndex(Transistor* connector) const;
    bool IsConnected(Transistor* connector) const;

    Int2 GetPos() const;
    void SetPos(Int2 newPos);
    void ChangePos(Int2 amount);

    bool SolderInput(Transistor* connection, Connection::Shape direction = Connection::Shape::XFirst);
    bool SolderOutput(Transistor* connection, Connection::Shape direction = Connection::Shape::XFirst);

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
    void Draw();
    void Highlight(Color color, float size) const;

    void FrameReset();

    void ClearReferences();

    friend void Save(const std::vector<Transistor*>&);
    friend void Load(std::vector<Transistor*>&);
};

extern std::vector<Transistor*> s_allTransistors;

char GateToSymbol(Transistor::Gate type);
Transistor::Gate SymbolToGate(char symbol);
