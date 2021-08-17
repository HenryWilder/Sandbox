#include <raylib.h>
#include <raymath.h>
#include <map>

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

    int column, row; // First & last 2 columns connect vertically; columns 3-7 (a-e) and 8-12 (f-j) connect horizontally, but not to each other

    bool OnBoard()
    {
        return column != -1;
    }

    operator Vector2()
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
    };
    virtual Type GetType() = 0;
};
struct Source : public Component
{
    Source(Port _start, int _power) : Component(_start), power(_power) {}
    int power; // Negative or positive * amount

    Type GetType() override
    {
        return Type::source;
    }
};
struct Wire : public Component
{
    Wire(Port _start, Port _end) : Component(_start), end(_end) {}
    Port end;

    Type GetType() override
    {
        return Type::wire;
    }
};
struct Resistor : public Wire
{
    Resistor(Port _start, Port _end, int _resistance) : Wire(_start, _end), resistance(_resistance) {}
    int resistance;

    Type GetType() override
    {
        return Type::resistor;
    }
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
Component*& ComponentAtPort(Port port)
{
    return g_breadBoard[port.row][port.column];
}

/*
* 
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 00000000000000    00 00000 00000 00
* 
*/
constexpr int c_portCountX = 14;
constexpr int c_portCountY = 30;
constexpr int c_portSize = 10;
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

void DrawPortBox(int x, int y, Color color)
{
    Port pos = BoardToScreen(x,y);
    DrawRectangle(pos.column + c_gutterSize, pos.row + c_gutterSize, c_portSize - c_gutterSize * 2, c_portSize - c_gutterSize * 2, color);
}
void DrawPortBox(Port pos, Color color)
{
    DrawPortBox(pos.column, pos.row, color);
}
void DrawPortLine(int x1, int y1, int x2, int y2, float thickness, Color color)
{
    Port p1 = BoardToScreen(x1,y1);
    Port p2 = BoardToScreen(x2,y2);
    DrawLineEx((Vector2)p1 + (float)c_portSize / 2.0f, (Vector2)p2 + (float)c_portSize / 2.0f, thickness, color);
}
void DrawPortLine(Port p1, Port p2, float thickness, Color color)
{
    DrawPortLine(p1.column, p1.row, p2.column, p2.row, thickness, color);
}

enum class Mode
{
    none,
    source,
    wire,
    resistor,
};

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
    Mode mode = Mode::wire;

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

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (hoveredPort.OnBoard())
            {
                if (selectedPort.OnBoard() && (mode == Mode::wire))
                {
                    Component*& comp = ComponentAtPort(selectedPort);
                    if (comp == 0)
                    {
                        if (selectedPort.column != hoveredPort.column || selectedPort.row != hoveredPort.row)
                            comp = new Wire(selectedPort, hoveredPort);
                    }
                    else
                    {
                        delete comp;
                        comp = 0;
                    }
                    selectedPort = { -1,-1 };
                }
                else
                {
                    selectedPort = hoveredPort;
                }
            }
            else
                selectedPort = { -1,-1 };
        }

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

            // Draw power flow
            {
                constexpr int halfway = c_portSize / 2;
                DrawPortLine(0,0,0,28,2.0f,RED);
                DrawPortLine(1,0,1,28,2.0f,BLACK);

                for (int y = 0; y < 30; ++y)
                {
                    for (int x = 0; x < 14; ++x)
                    {
                        if (Component* comp = g_breadBoard[y][x])
                        {
                            if (Wire* wire = dynamic_cast<Wire*>(comp))
                            {
                                if (wire->start.column == 0 || wire->start.column == 12)
                                {
                                    if (wire->end.column < 7)
                                    {
                                        DrawPortLine(2, wire->end.row, 6, wire->end.row, 2.0f, RED);
                                    }
                                    else
                                    {
                                        DrawPortLine(7, wire->end.row, 11, wire->end.row, 2.0f, RED);
                                    }
                                }
                                else if (wire->end.column == 0 || wire->end.column == 12)
                                {
                                    if (wire->start.column < 7)
                                    {
                                        DrawPortLine(2, wire->start.row, 6, wire->start.row, 2.0f, RED);
                                    }
                                    else
                                    {
                                        DrawPortLine(7, wire->start.row, 11, wire->start.row, 2.0f, RED);
                                    }
                                }
                                else if (wire->start.column == 1 || wire->start.column == 13)
                                {
                                    if (wire->end.column < 7)
                                    {
                                        DrawPortLine(2, wire->end.row, 6, wire->end.row, 2.0f, BLACK);
                                    }
                                    else
                                    {
                                        DrawPortLine(7, wire->end.row, 11, wire->end.row, 2.0f, BLACK);
                                    }
                                }
                                else if (wire->end.column == 1 || wire->end.column == 13)
                                {
                                    if (wire->start.column < 7)
                                    {
                                        DrawPortLine(2, wire->start.row, 6, wire->start.row, 2.0f, BLACK);
                                    }
                                    else
                                    {
                                        DrawPortLine(7, wire->start.row, 11, wire->start.row, 2.0f, BLACK);
                                    }
                                }
                            }
                        }
                    }
                }
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
                            {
                                DrawRectangle(start.column + c_gutterSize, start.row + c_gutterSize, c_portSize - 2 * c_gutterSize, c_portSize - 2 * c_gutterSize, (source->power < 0 ? BLACK : RED));
                            }
                            break;

                        case Component::Type::wire:
                            if (Wire* wire = dynamic_cast<Wire*>(comp))
                            {
                                Port end = BoardToScreen(wire->end.column, wire->end.row);
                                Vector2 startv = { (float)start.column + (float)c_portSize / 2.0f, (float)start.row + (float)c_portSize / 2.0f };
                                Vector2 endv = { (float)end.column + (float)c_portSize / 2.0f, (float)end.row + (float)c_portSize / 2.0f };
                                DrawLineEx(startv, endv, 2.0f, LIGHTGRAY);
                                int wireLength = roundf(Vector2Distance(startv, endv)) / (float)c_portSize;
                                if (wireLength > 1)
                                {
                                    Vector2 jacketStartv = startv + Vector2Normalize(endv - startv) * 4.0f;
                                    Vector2 jacketEndv = endv + Vector2Normalize(startv - endv) * 4.0f;
                                    Color jacketColor;
                                    switch (wireLength)
                                    {
                                    case 2: jacketColor = RED; break;
                                    case 3: jacketColor = ORANGE; break;
                                    case 4: jacketColor = YELLOW; break;
                                    case 5: jacketColor = LIME; break;
                                    case 6: jacketColor = DARKBLUE; break;
                                    case 7: jacketColor = DARKGRAY; break;
                                    case 8: jacketColor = LIME; break;
                                    case 9: jacketColor = DARKBLUE; break;
                                    case 10: jacketColor = WHITE; break;
                                    default: jacketColor = LIME; break;
                                    }
                                    DrawLineEx(jacketStartv, jacketEndv, 4.0f, jacketColor);
                                }
                            }
                            break;

                        case Component::Type::resistor:
                            if (Resistor* resistor = dynamic_cast<Resistor*>(comp))
                            {
                                Port end = BoardToScreen(resistor->end.column, resistor->end.row);
                                DrawLine(start.column + c_portSize / 2, start.row + c_portSize / 2, end.column + c_portSize / 2, end.row + c_portSize / 2, ORANGE);
                            }
                            break;

                        default:
                            DrawRectangle(start.column + c_gutterSize, start.row + c_gutterSize, c_portSize - 2 * c_gutterSize, c_portSize - 2 * c_gutterSize, MAGENTA);
                            break;
                        }
                    }
                }
            }

            if (selectedPort.OnBoard())
            {
                Port pos = BoardToScreen(selectedPort.column, selectedPort.row);
                DrawRectangle(pos.column + c_gutterSize, pos.row + c_gutterSize, c_portSize - 2 * c_gutterSize, c_portSize - 2 * c_gutterSize, BLUE);
            }
            if (hoveredPort.OnBoard())
            {
                Port pos = BoardToScreen(hoveredPort.column, hoveredPort.row);
                DrawRectangle(pos.column + c_gutterSize, pos.row + c_gutterSize, c_portSize - 2 * c_gutterSize, c_portSize - 2 * c_gutterSize, SKYBLUE);
            }

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
                delete g_breadBoard[y][x];
        }
    }

    CloseWindow();

	return 0;
}