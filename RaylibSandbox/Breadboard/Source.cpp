#include <raylib.h>
#include <raymath.h>
#include <map>
#include <unordered_set>

#define NO_COLOR { 0,0,0,0 }

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#pragma endregion

/******************************************************
* 
*       +- abcde fghij +-       +- abcde fghij +-
*     1 oo ooooo ooooo oo     1 || ----- ----- ||
*     2 oo ooooo ooooo oo     2 || ----- ----- ||
*     3 oo ooooo ooooo oo     3 || ----- ----- ||
*     4 oo ooooo ooooo oo     4 || ----- ----- ||
*     5 oo ooooo ooooo oo     5 || ----- ----- ||
*     6    ooooo ooooo        6 || ----- ----- ||
*     7 oo ooooo ooooo oo     7 || ----- ----- ||
*     8 oo ooooo ooooo oo     8 || ----- ----- ||
*     9 oo ooooo ooooo oo     9 || ----- ----- ||
*    10 oo ooooo ooooo oo    10 || ----- ----- ||
*    11 oo ooooo ooooo oo    11 || ----- ----- ||
*    12    ooooo ooooo       12 || ----- ----- ||
*    13 oo ooooo ooooo oo    13 || ----- ----- ||
*    14 oo ooooo ooooo oo    14 || ----- ----- ||
*    15 oo ooooo ooooo oo    15 || ----- ----- ||
*    16 oo ooooo ooooo oo    16 || ----- ----- ||
*    17 oo ooooo ooooo oo    17 || ----- ----- ||
*    18    ooooo ooooo       18 || ----- ----- ||
*    19 oo ooooo ooooo oo    19 || ----- ----- ||
*    20 oo ooooo ooooo oo    20 || ----- ----- ||
*    21 oo ooooo ooooo oo    21 || ----- ----- ||
*    22 oo ooooo ooooo oo    22 || ----- ----- ||
*    23 oo ooooo ooooo oo    23 || ----- ----- ||
*    24    ooooo ooooo       24 || ----- ----- ||
*    25 oo ooooo ooooo oo    25 || ----- ----- ||
*    26 oo ooooo ooooo oo    26 || ----- ----- ||
*    27 oo ooooo ooooo oo    27 || ----- ----- ||
*    28 oo ooooo ooooo oo    28 || ----- ----- ||
*    29 oo ooooo ooooo oo    29 || ----- ----- ||
*    30    ooooo ooooo       30 || ----- ----- ||
* 
******************************************************/
struct Port
{
    Port() = default;
    Port(int x, int y) : column(x), row(y) {}
    Port(Vector2 vec) : column(vec.x), row(vec.y) {}
    Port(Port& port) : column(port.column), row(port.row) {}

    int column, row; // First & last 2 columns connect vertically; columns 3-7 (a-e) and 8-12 (f-j) connect horizontally, but not to each other

    bool OnBoard() const
    {
        return column != -1;
    }

    Port& operator=(Port other)
    {
        column = other.column;
        row = other.row;
        return *this;
    }
    bool operator==(Port other) const
    {
        return other.column == column && other.row == row;
    }
    bool operator!=(Port other) const
    {
        return other.column != column || other.row != row;
    }
    operator Vector2() const
    {
        return { (float)column, (float)row };
    }
};
struct Component
{
    Component(Port _start) : start(_start) {}
    Port start;

    enum class Type
    {
        source,
        wire,
        resistor,
        LED,
    };
    virtual Type GetType() = 0;
    virtual void Draw() = 0;
};
struct Source : public Component
{
    Source(Port _start, int _power) : Component(_start), power(_power) {}
    float power; // Negative or positive * amount

    Type GetType() override
    {
        return Type::source;
    }
    virtual void Draw() override;
};
struct Wire : public Component
{
    Wire(Port _start, Port _end) : Component(_start), end(_end) {}
    Port end;

    Type GetType() override
    {
        return Type::wire;
    }
    virtual void Draw() override;
};
struct Resistor : public Wire
{
    Resistor(Port _start, Port _end, float _resistance) : Wire(_start, _end), resistance(_resistance) {}
    float resistance;

    Type GetType() override
    {
        return Type::resistor;
    }
    virtual void Draw() override;
};
struct LED : public Wire
{
    LED(Port _start, Port _end, Color _color) : Wire(_start, _end), color(_color) {}

    Color color; // Color of the light when on

    Type GetType() override
    {
        return Type::LED;
    }
    virtual void Draw() override;
};
Component* g_breadBoard[30][14] =
{
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    {  0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0  },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    {  0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0  },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    {  0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0  },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    {  0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0  },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    { 00,00, 0,0,0,0,0, 0,0,0,0,0, 00,00 },
    {  0,0,  0,0,0,0,0, 0,0,0,0,0,  0,0  },
};
Component*& GetComponentAtPort(Port port)
{
    return g_breadBoard[port.row][port.column];
}

constexpr int c_portCountX = 14;
constexpr int c_portCountY = 30;
constexpr int c_portSize = 20;
constexpr int c_gutterSize = 1;
constexpr int c_deadspaceX = 10;
constexpr int c_deadspaceY = 10;
Port BoardFromScreen(int x, int y)
{
    x = x / c_portSize - c_deadspaceX;
    y = y / c_portSize - c_deadspaceY;

    if (x == 2 || x == 8 || x == 14 ||
        x > (c_portCountX + 2) || x < 0 ||
        y > (c_portCountY - 1) || y < 0 ||
        ((y % 6 == 5) && (x < 2 || x > 13)))
        return { -1, -1 };

    x -= (x > 2 ? (x > 8 ? (x > 14 ? 3 : 2) : 1) : 0);
    return { x, y };
}
Port BoardFromScreen(Port port) { return BoardFromScreen(port.column, port.row); }
Port BoardToScreen(int x, int y)
{
    x += (x >= 2 ? (x >= 7 ? (x >= 12 ? 3 : 2) : 1) : 0);
    return { c_portSize * (x + c_deadspaceX), c_portSize * (y + c_deadspaceY) };
}
Port BoardToScreen(Port port) { return BoardToScreen(port.column, port.row); }

Vector2 PortCenter(int x, int y)
{
    Port pos = BoardToScreen(x,y);
    return (Vector2)pos + (float)c_portSize / 2.0f;
}
Vector2 PortCenter(Port port)
{
    Port pos = BoardToScreen(port);
    return (Vector2)pos + (float)c_portSize / 2.0f;
}

void DrawPortBox(int x, int y, Color color)
{
    Port pos = BoardToScreen(x,y);
    DrawRectangle(pos.column + c_gutterSize, pos.row + c_gutterSize, c_portSize - c_gutterSize * 2, c_portSize - c_gutterSize * 2, color);
}
void DrawPortBox(Port pos, Color color)
{
    DrawPortBox(pos.column, pos.row, color);
}
void Source::Draw()
{
    DrawPortBox(start, (power > 0.0f ? RED : BLACK));
}
void DrawPortLine(int x1, int y1, int x2, int y2, float thickness, Color color)
{
    DrawLineEx(PortCenter(x1,y1), PortCenter(x2, y2), thickness, color);
}
void DrawPortLine(Port p1, Port p2, float thickness, Color color)
{
    DrawLineEx(PortCenter(p1), PortCenter(p2), thickness, color);
}

struct Section
{
    Section(int _x, int _y)
    {
        if ((_x >= 0 && _x < 2) || (_x >= 12 && _x < 14))
        {
            x = _x;
            y = 0;
            width = 1;
            height = 29;
        }
        else if (_x >= 2 && _x < 7)
        {
            x = 2;
            y = _y;
            width = 5;
            height = 1;
        }
        else  if (_x >= 7 && _x < 12)
        {
            x = 7;
            y = _y;
            width = 5;
            height = 1;
        }
        else
        {
            x = -1;
            y = -1;
            width = 0;
            height = 0;
        }
    }
    Section(Port port)
    {
        *this = Section(port.column, port.row);
    }

    int x, y, width, height;

    void Power(float power) const;
    void DrawPowerLine(float thick) const;
};
const Section g_allSections[] =
{
    Section(0,0), Section(1,0), Section(2, 0), Section(7, 0),
                                Section(2, 1), Section(7, 1),
                                Section(2, 2), Section(7, 2),
                                Section(2, 3), Section(7, 3),
                                Section(2, 4), Section(7, 4),
                                Section(2, 5), Section(7, 5),
                                Section(2, 6), Section(7, 6),
                                Section(2, 7), Section(7, 7),
                                Section(2, 8), Section(7, 8),
                                Section(2, 9), Section(7, 9),
                                Section(2,10), Section(7,10),
                                Section(2,11), Section(7,11),
                                Section(2,12), Section(7,12),
                                Section(2,13), Section(7,13),
                                Section(2,14), Section(7,14),
                                Section(2,15), Section(7,15),
                                Section(2,16), Section(7,16),
                                Section(2,17), Section(7,17),
                                Section(2,18), Section(7,18),
                                Section(2,19), Section(7,19),
                                Section(2,20), Section(7,20),
                                Section(2,21), Section(7,21),
                                Section(2,22), Section(7,22),
                                Section(2,23), Section(7,23),
                                Section(2,24), Section(7,24),
                                Section(2,25), Section(7,25),
                                Section(2,26), Section(7,26),
                                Section(2,27), Section(7,27),
                                Section(2,28), Section(7,28),
                                Section(2,29), Section(7,29), Section(12,0), Section(13,0),
};

struct BreadBoardPower
{
private:
    float v[4];
    float h[2][30];

public:
    void ClearPower()
    {
        for (float& p : v)
        {
            p = 0.0f;
        }
        for (int i = 0; i < 2; ++i)
        {
            for (float& p : h[i])
            {
                p = 0.0f;
            }
        }
    }
    float SamplePower(Port at) const
    {
        Section sect(at);

        if (sect.x == 0)
            return v[0];
        else if (sect.x == 1)
            return v[1];
        else if (sect.x == 12)
            return v[2];
        else if (sect.x == 13)
            return v[3];

        else if (sect.x == 2)
            return h[0][sect.y];
        else if (sect.x == 7)
            return h[1][sect.y];
    }
    void AddPower(Port at, float amt)
    {
        Section sect(at);

        if (sect.x == 0)
            v[0] += amt;
        else if (sect.x == 1)
            v[1] += amt;
        else if (sect.x == 12)
            v[2] += amt;
        else if (sect.x == 13)
            v[3] += amt;

        else if (sect.x == 2)
            h[0][sect.y] += amt;
        else if (sect.x == 7)
            h[1][sect.y] += amt;
    }
} g_power;

void Section::Power(float power) const // true = + false = -
{
    g_power.AddPower({ x,y }, power);
}
void Section::DrawPowerLine(float thick) const
{
    float val = g_power.SamplePower({ x,y });
    if (val)
    {
        Vector2 p1 = PortCenter(x, y);
        Vector2 p2 = PortCenter(x + width - 1, y + height - 1);

        DrawLineEx(p1, p2, thick, ColorAlpha(BLACK, abs(val)));

        /*
        Vector2 direction = Vector2Normalize(p2 - p1);

        if ((long)(GetTime() * 2.0f) & 1)
            p1 += direction * 2.0f;

        Vector2 size = Vector2{ thick, thick };
        Vector2 halfsize = size * 0.5f;

        for (float i = 0.0f; i < Vector2Distance(p1, p2); i += 4.0f)
        {
            DrawRectangleV(p1 + direction * i - halfsize, size, YELLOW);
        }
        */
    }
}

void DrawBreadLine(int x, int y, Color color)
{
    if (x < 7)
        DrawPortLine(2, y, 6, y, 2.0f, color);
    else if (x < 12)
        DrawPortLine(7, y, 11, y, 2.0f, color);
    else
        DrawPortLine(x, 0, x, 28, 2.0f, color);
}

void DrawWire(Wire* wire)
{
    static Color jacketColors[] = {
        NO_COLOR,
        NO_COLOR,
        RED,
        ORANGE,
        YELLOW,
        LIME,
        DARKBLUE,
        DARKGRAY,
        LIME,
        DARKBLUE,
        WHITE,
        LIME,
    };

    Vector2 start = PortCenter(wire->start);
    Vector2 end = PortCenter(wire->end);

    DrawLineEx(start, end, 2.0f, LIGHTGRAY);
    int wireLength = roundf(Vector2Distance(start, end)) / (float)c_portSize;
    if (wireLength > 1)
    {
        Vector2 jacketStartv = start + Vector2Normalize(end - start) * 4.0f;
        Vector2 jacketEndv = end + Vector2Normalize(start - end) * 4.0f;
        DrawLineEx(jacketStartv, jacketEndv, 4.0f, jacketColors[std::min(wireLength, 11)]);
    }
}
void Wire::Draw()
{
    DrawWire(this);
}

void DrawResistor(Resistor* resistor)
{
    Vector2 start = PortCenter(resistor->start);
    Vector2 end = PortCenter(resistor->end);

    DrawLineEx(start, end, 2.0f, LIGHTGRAY);
    int wireLength = roundf(Vector2Distance(start, end)) / (float)c_portSize;
    if (wireLength > 1)
    {
        Vector2 midpoint = (start + end) / 2.0f;
        Vector2 resistStart = midpoint + Vector2Normalize(end - start) * 4.0f;
        Vector2 resistEnd = midpoint + Vector2Normalize(start - end) * 4.0f;
        DrawLineEx(resistStart, resistEnd, 6.0f, BROWN);
    }
}
void Resistor::Draw()
{
    DrawResistor(this);
}

void DrawLED(LED* led)
{
    Vector2 start = PortCenter(led->start);
    Vector2 end = PortCenter(led->end);
    Vector2 midpoint = (start + end) / 2.0f;
    DrawCircleV(midpoint, c_portSize / 2.0f, LIGHTGRAY);
    DrawCircleV(midpoint, c_portSize / 2.0f, ColorAlpha(led->color, g_power.SamplePower(led->start) > 0.0 ? 0.9f : 0.25f));
}
void LED::Draw()
{
    DrawLED(this);
}

std::unordered_set<size_t> g_avoid;
size_t SerializePort(Port port)
{
    return 0ull | (((size_t)port.column << 040) | (size_t)port.row);
}

bool FindConnections(Port from, float power)
{
    Section sect(from);
    sect.Power(power);
    for (int y = sect.y; y < (sect.y + sect.height); ++y)
    {
        for (int x = sect.x; x < (sect.x + sect.width); ++x)
        {
            Port pos = { x,y };

            if (g_avoid.find(SerializePort(pos)) != g_avoid.end())
                continue;

            if (Component* comp = GetComponentAtPort(pos))
            {
                if (Wire* wire = dynamic_cast<Wire*>(comp))
                {
                    g_avoid.insert(SerializePort(wire->start));
                    g_avoid.insert(SerializePort(wire->end));

                    if (Resistor* resistor = dynamic_cast<Resistor*>(wire))
                        power *= resistor->resistance;

                    return FindConnections((wire->start == pos ? wire->end : wire->start), power);
                }
                else if (Source* source = dynamic_cast<Source*>(comp))
                {
                    if (sign(source->power) != sign(power))
                    {
                        return true;
                    }
                    else return false;
                }
            }
        }
    }
}

int main()
{
    int windowWidth = (2 * c_deadspaceX + c_portCountX + 3) * c_portSize;
    int windowHeight = (2 * c_deadspaceY + c_portCountY) * c_portSize;
    InitWindow(windowWidth, windowHeight, "Breadboard");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    Port selectedPort = { -1,-1 };

    enum class Mode {
        none,
        source,
        wire,
        resistor,
        LED,
    } mode = Mode::wire;

    g_breadBoard[0][0] = new Source({0,0}, 1);
    g_breadBoard[1][1] = new Source({1,1}, -1);

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        Port hoveredPort;
        {
            Vector2 mouse = GetMousePosition();
            hoveredPort = BoardFromScreen((int)truncf(mouse.x), (int)truncf(mouse.y));
        }

        if (IsKeyPressed(KEY_ONE))
            mode = Mode::source;
        if (IsKeyPressed(KEY_TWO))
            mode = Mode::wire;
        if (IsKeyPressed(KEY_THREE))
            mode = Mode::resistor;
        if (IsKeyPressed(KEY_FOUR))
            mode = Mode::LED;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) // On left click
        {
            if (hoveredPort.OnBoard()) // If the mouse is on a valid position
            {
                if ((mode == Mode::wire || mode == Mode::resistor || mode == Mode::LED) && selectedPort.OnBoard()) // If currently creating a wire & the wire's start point is valid
                {
                    Component*& fcomp = GetComponentAtPort(selectedPort); // Start
                    Component*& scomp = GetComponentAtPort(hoveredPort); // End

                    if (!fcomp && !scomp) // If both spaces are free
                    {
                        if (selectedPort.column != hoveredPort.column || selectedPort.row != hoveredPort.row)
                        {
                            switch (mode)
                            {
                            case Mode::none:
                                fcomp = 0;
                                break;
                            case Mode::source:
                                break;
                            case Mode::wire:
                                fcomp = new Wire(selectedPort, hoveredPort);
                                break;
                            case Mode::resistor:
                                fcomp = new Resistor(selectedPort, hoveredPort, 0.5f);
                                break;
                            case Mode::LED:
                                fcomp = new LED(selectedPort, hoveredPort, RED);
                                break;
                            default:
                                break;
                            }
                            GetComponentAtPort(hoveredPort) = fcomp;
                        }
                    }
                    else if (selectedPort == hoveredPort)
                    {
                        if (fcomp)
                        {
                            if (Wire* wire = dynamic_cast<Wire*>(fcomp))
                            {
                                GetComponentAtPort(wire->start) = 0;
                                GetComponentAtPort(wire->end) = 0;
                                delete wire;
                            }
                            else
                            {
                                delete fcomp;
                                fcomp = 0;
                            }
                        }
                        if (scomp)
                        {
                            if (Wire* wire = dynamic_cast<Wire*>(scomp))
                            {
                                GetComponentAtPort(wire->start) = 0;
                                GetComponentAtPort(wire->end) = 0;
                                delete wire;
                            }
                            else
                            {
                                delete scomp;
                                scomp = 0;
                            }
                        }
                    }
                    selectedPort = { -1,-1 };
                }
                // a) Not currently creating a wire
                // b) Wire start point is invalid
                // c) Both a and b
                else
                {
                    selectedPort = hoveredPort;
                }
            }
            else // The mouse is outside of the board
            {
                selectedPort = { -1,-1 };
            }
        }

        g_power.ClearPower();
        g_avoid.clear();
        FindConnections({ 1,1 }, -1.0f);

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(RAYWHITE);

            // Draw spaces
            for (int y = 0; y < 30; ++y)
            {
                for (int x = 0; x < 14; ++x)
                {
                    if ((y % 6 == 5) && (x < 2 || x > 11))
                        continue;

                    DrawPortBox(x, y, GRAY);
                }
            }

            if (selectedPort.OnBoard())
            {
                DrawPortBox(selectedPort, BLUE);
            }
            if (hoveredPort.OnBoard())
            {
                DrawPortBox(hoveredPort, SKYBLUE);
            }
            if (selectedPort.OnBoard() && hoveredPort.OnBoard())
                DrawPortLine(selectedPort, hoveredPort, 4.0f, ColorAlpha(LIGHTGRAY, 0.5));

            for (Section sect : g_allSections)
            {
                sect.DrawPowerLine(2.0f);
            }

            // Draw components
            for (int y = 0; y < 30; ++y)
            {
                for (int x = 0; x < 14; ++x)
                {
                    if ((y % 6 == 5) && (x < 2 || x > 11))
                        continue;

                    if (g_breadBoard[y][x])
                    {
                        Component* comp = g_breadBoard[y][x];
                        Port start = BoardToScreen(comp->start.column, comp->start.row);
                        switch (comp->GetType())
                        {
                        case Component::Type::source:
                            if (Source* source = dynamic_cast<Source*>(comp))
                                DrawRectangle(start.column + c_gutterSize, start.row + c_gutterSize, c_portSize - 2 * c_gutterSize, c_portSize - 2 * c_gutterSize, (source->power < 0 ? BLACK : RED));
                            break;

                        case Component::Type::wire:
                            if (Wire* wire = dynamic_cast<Wire*>(comp))
                                DrawWire(wire);
                            break;

                        case Component::Type::resistor:
                            if (Resistor* resistor = dynamic_cast<Resistor*>(comp))
                                DrawResistor(resistor);
                            break;

                        case Component::Type::LED:
                            if (LED* led = dynamic_cast<LED*>(comp))
                            {
                                DrawLED(led);
                            }
                            break;

                        default:
                            DrawRectangle(start.column + c_gutterSize, start.row + c_gutterSize, c_portSize - 2 * c_gutterSize, c_portSize - 2 * c_gutterSize, MAGENTA);
                            break;
                        }
                    }
                }
            }

            const char* modeString;
            switch (mode)
            {
            default:
            case Mode::none: modeString = ""; break;
            case Mode::source: modeString = "source"; break;
            case Mode::wire: modeString = "wire"; break;
            case Mode::resistor: modeString = "resistor"; break;
            case Mode::LED: modeString = "LED"; break;
            }
            DrawText(modeString, 0,0,18,MAGENTA);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (int y = 0; y < 30; ++y)
    {
        for (int x = 0; x < 14; ++x)
        {
            if (g_breadBoard[y][x] != 0)
            {
                if (Wire* wire = dynamic_cast<Wire*>(g_breadBoard[y][x])) // Has two points sharing a wire pointer
                {
                    GetComponentAtPort(wire->start) = 0;
                    GetComponentAtPort(wire->end) = 0;
                    delete wire;
                }
                else // Only has one point
                    delete g_breadBoard[y][x];
            }
        }
    }

    CloseWindow();

	return 0;
}