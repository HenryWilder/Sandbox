#pragma once
#include <vector>
#include "IDSystem.h"

class Transistor;

struct ID_t // Hhhhhhhhhhhhhh
{
private:
    uint32_t id;

public:
    constexpr ID_t() : id(0) {};
    constexpr ID_t(uint32_t _id) : id(_id) {};

    operator uint32_t();
    Transistor* operator->();
    Transistor* operator*();
    ID_t& operator=(ID_t b);
    operator bool();
    bool operator==(ID_t b);
    bool operator!=(ID_t b);
};

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
private:
    bool ANY(std::vector<Transistor*>& in);
    bool NOR(std::vector<Transistor*>& in);
    bool AND(std::vector<Transistor*>& in);
    bool XOR(std::vector<Transistor*>& in);

public:
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
        Connection() : shape{ Shape::XFirst }, connector{ } {};
        Connection(ID_t _connector) : shape{ Shape::XFirst }, connector{ _connector } {};
        Connection(ID_t _connector, Shape _shape) : shape{ (_shape == Shape::Size ? Shape::XFirst : _shape) }, connector{ _connector } {};

        Shape shape;
        ID_t connector;

        void IncrementShape();
        static Shape NextShape(Shape);
    };

    enum Flag : unsigned char {
        DRAWN_TF = 0b00001,
        EVALUATED_TF = 0b00010,
        BEING_EVALUATED = 0b00100,
        EVALUATION = 0b01000,
        IS_HIDDEN = 0b10000,
    };
    static Flag operator|(Flag a, Flag b);

    Transistor()
        : prev{ }, next{ }, pos( ), type( Gate::Simple ), flags(0), containingComponent(nullptr) {};
    Transistor(Int2 _pos, Gate _type = Gate::Simple)
        : prev{ }, next{ }, pos( _pos ), type( _type ), flags(0), containingComponent(nullptr) {};

    static IDMap<ID_t, Transistor*> s_transistorMap; // Map of all transistors (as validators). This is used in place of pointers to transistors themselves.

private:
    ID_t myID;
    std::vector<Connection> prev;
    std::vector<Connection> next;
    Int2 pos;
    Gate type;
    unsigned char flags;
    Component* containingComponent;

public:
    static Transistor* Get(ID_t id);
    static ID_t Create(Int2 _pos = Int2(), Gate _type = Gate::Simple);
    static void Destroy(const ID_t id);

    unsigned char TestFlags(unsigned char flags) const;
    unsigned char TestFlags(Flag flags) const;
    void SetFlags(unsigned char flags, unsigned char set);
    void SetFlags(Flag flags, unsigned char set);
    void SetFlags(unsigned char flags, bool set);
    void SetFlags(Flag flags, bool set);

    ID_t GetID();

private:
    std::vector<Transistor*> Validate(std::vector<Connection>& arr);
    void Evaluate();

    static Int2 JointPos(Int2 start, Int2 end, Connection::Shape direction);

public:
    Gate GetGateType() const;
    void SetGateType(Gate _type);
    void IncrementGate();
    Component* GetComponent() const;
    void SetComponent(Component* component);
    
    void Hide();
    void UnHide();

    Int2 GetPos() const;
    void SetPos(Int2 newPos);
    void ChangePos(Int2 amount);

    bool SolderInput(ID_t connection, Connection::Shape direction = Connection::Shape::XFirst);
    bool SolderOutput(ID_t connection, Connection::Shape direction = Connection::Shape::XFirst);
    bool DesolderInput(int which);
    bool DesolderInput(ID_t what);
    bool DesolderOutput(int which);
    bool DesolderOutput(ID_t what);

    bool OutputOnly() const;    // Is the start of a tree
    bool InputOnly() const;     // Is the end of a tree
    bool ConnectsExternally() const;
    bool IsDisconnected() const;

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

    friend void Save(const std::vector<ID_t>&);
    friend void Load(std::vector<ID_t>&);
};

char GateToSymbol(Transistor::Gate type);
Transistor::Gate SymbolToGate(char symbol);
