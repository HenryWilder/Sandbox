#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <memory>

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

enum class Item : unsigned int
{
    null = 0,
    iron_ore = 100,
};
struct ItemSlot
{
    Item item = Item::null;
    unsigned int count = 0u;
};

enum class Side : unsigned char { null, input, output };
enum class Shape : bool { part, fluid };

struct Building;
struct Port;
struct Port
{
    static constexpr int s_width = 16;
    static constexpr int s_length = 4;

    Building* of; // Back pointer
    unsigned int offset; // Position in respective array
    Side side; // Which array it is in
    Shape takes = Shape::part; // Whether this takes fluids or solids
    Port* to = nullptr; // Simply a connector, not at all owned by this object
    Rectangle shape = Rectangle{};

    Vector2 Center() const
    {
        return {
            shape.x + s_width / 2,
            shape.y + s_length / 2
        };
    }
};
struct Building
{
    enum class Type : unsigned short
    {
        miner,
        constructor,
        assembler,
        merger,
        manufacturer,

        refinery,
        coal_burner,
        storage,
        splitter,
    };
    Building(Type type)
    {
        switch (type)
        {
        case Type::miner:
            break;

        default:
            i.resize(1); break;

        case Type::refinery:
            i.resize(2); o.resize(2);
            i[1].takes = o[1].takes = Shape::fluid;
            break;

        case Type::coal_burner:
            i.resize(2); i[1].takes = Shape::fluid;  break;

        case Type::constructor:
        case Type::assembler:
        case Type::merger:
        case Type::manufacturer:
            i.resize((size_t)type); break;
        }
        switch (type)
        {
        case Type::refinery:
        case Type::coal_burner:
            break;

        default:
            o.resize(1); break;

        case Type::splitter:
            o.resize(3); break;
        }

        // Inventory
        if (type == Type::storage)
            inv.resize(24);
        else if (type != Type::merger && type != Type::splitter)
            inv.resize((size_t)(i.size() + o.size()));
        
        // Initializing
        for (int n = 0; n < i.size(); ++n)
        {
            i[n].of = this;
            i[n].offset = n;
            i[n].side = Side::input;
        }
        for (int n = 0; n < o.size(); ++n)
        {
            o[n].of = this;
            o[n].offset = n;
            o[n].side = Side::output;
        }
    }

    static constexpr int s_padding = 4;
    static constexpr int s_length = 32;

    std::vector<Port> i;
    std::vector<Port> o;
    std::vector<ItemSlot> inv;
    Rectangle shape = Rectangle{};
};


// Returns true on success (will not connect if connections are full)
// src must be on output side, dest must be on input side
bool Connect(Port* a, Port* b)
{
    if (!a || !b || a->to || b->to)
        return false;

    a->to = b;
    b->to = a;
    return true;
}
void Disconnect(Port* port)
{
    if (port && port->to)
    {
        port->to->to = nullptr;
        port->to = nullptr;
    }
}

float Pad(float n, float size, float padding)
{
    return (n * (size + padding)) + padding;
}
Rectangle RectangleAtPosition(float width, float height, float originx, float originy)
{
    return Rectangle{
        (originx - width / 2.0f),
        (originy - height / 2.0f),
        width,
        height
    };
}
float PadPort(float n)
{
    return Pad(n, (float)Port::s_width, (float)Building::s_padding);
}
Rectangle PortRectangleAtPosition(float originx, float originy)
{
    return RectangleAtPosition(Port::s_width, Port::s_length, originx, originy);
}

void MoveBuilding(Building* building, Vector2 center)
{
    bool inputHeavy = building->i.size() > building->o.size();
    int heavySizePortCount = (inputHeavy ? building->i.size() : building->o.size());
    int lightSizePortCount = (inputHeavy ? building->o.size() : building->i.size());
    int size = PadPort(heavySizePortCount);
    building->shape = RectangleAtPosition(size, Building::s_length, center.x, center.y);

    float sidesDifference = (float)(heavySizePortCount - lightSizePortCount) / 2.0f;

    for (int i = 0; i < building->i.size(); ++i)
    {
        building->i[i].shape = PortRectangleAtPosition(building->shape.x + PadPort(i + (!inputHeavy * sidesDifference)) + Port::s_width / 2, building->shape.y);
    }
    for (int o = 0; o < building->o.size(); ++o)
    {
        building->o[o].shape = PortRectangleAtPosition(building->shape.x + PadPort(o + (inputHeavy * sidesDifference)) + Port::s_width / 2, building->shape.y + Building::s_length);
    }
}
void DrawBuilding(Building* building)
{
    DrawRectangleRec(building->shape, DARKGRAY);

    for (Port& i : building->i)
    {
        DrawRectangleRec(i.shape, ORANGE);
    }
    for (Port& o : building->o)
    {
        DrawRectangleRec(o.shape, LIME);
    }
}
void DrawConnection(Port* a, Port* b)
{
    DrawLineEx({ a->shape.x + a->s_width / 2, a->shape.y + a->s_length / 2 }, { b->shape.x + b->s_width / 2, b->shape.y + b->s_length / 2 }, 3, BLUE);
}
void DrawConnection(Port* connection)
{
    if (connection && connection->to)
        DrawConnection(connection, connection->to);
}

struct Hover
{
    Building* building = nullptr;
    Side side = Side::null;
    int port = -1;

    void Reset()
    {
        building = nullptr;
        side = Side::null;
        port = -1;
    }

    Port* GetPort()
    {
        switch (side)
        {
        case Side::null:
            return nullptr;
        case Side::input:
            return &building->i[port];
        case Side::output:
            return &building->o[port];
        }
    }

    bool BuildingEmpty()
    {
        return !building;
    }
    bool PortEmpty()
    {
        return (side == Side::null) || (port == -1);
    }

    void SetAsInput(int n)
    {
        side = Side::input;
        port = n;
    }
    void SetAsOutput(int n)
    {
        side = Side::output;
        port = n;
    }

    bool IsInput()
    {
        return side == Side::input;
    }
    bool IsOutput()
    {
        return side == Side::output;
    }
};

void DrawBuildingHighlighted(Hover* hover)
{
    if (hover->building)
    {
        DrawRectangleRec(hover->building->shape, GRAY);

        for (Port& i : hover->building->i)
        {
            DrawRectangleRec(i.shape, ORANGE);
        }
        for (Port& o : hover->building->o)
        {
            DrawRectangleRec(o.shape, LIME);
        }
        if (hover->IsInput())
            DrawRectangleRec(hover->building->i[hover->port].shape, YELLOW);
        else if (hover->IsOutput())
            DrawRectangleRec(hover->building->o[hover->port].shape, GREEN);
    }
}

bool CheckCollisionPointLine(Vector2 point, Vector2 startPos, Vector2 endPos, int lineWidth)
{
    Vector2 perpendicular = startPos - endPos;
    perpendicular = Vector2Normalize(perpendicular);
    perpendicular = Vector2Rotate(perpendicular, 90.0f);
    perpendicular *= (float)(lineWidth / 2);
    return CheckCollisionLines(startPos, endPos, point - perpendicular, point + perpendicular, &point);
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "My Raylib Program");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    std::vector<Building*> buildings;
    int clickNumber = 0;

    Hover hovering;
    Hover connectBuff;
    Building::Type buildingType = Building::Type::assembler;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        hovering.Reset();
        // Locate hovered building
        for (Building* building : buildings)
        {
            Rectangle paddedCollision{
                building->shape.x,
                building->shape.y - Port::s_length / 2,
                building->shape.width,
                building->shape.height + Port::s_length
            };
            if (CheckCollisionPointRec(GetMousePosition(), paddedCollision))
            {
                hovering.building = building;
                break;
            }
        }
        // Locate hovered port
        if (!hovering.BuildingEmpty())
        {
            for (int i = 0; hovering.PortEmpty() && i < hovering.building->i.size(); ++i)
            {
                if (CheckCollisionPointRec(GetMousePosition(), hovering.building->i[i].shape))
                {
                    hovering.SetAsInput(i);
                    break;
                }
            }

            for (int i = 0; hovering.PortEmpty() && i < hovering.building->o.size(); ++i)
            {
                if (CheckCollisionPointRec(GetMousePosition(), hovering.building->o[i].shape))
                {
                    hovering.SetAsOutput(i);
                    break;
                }
            }
        }

        // Locate hovered wire
        if (hovering.BuildingEmpty())
        {
            for (Building* building : buildings)
            {
                for (const Port& port : building->o)
                {
                    if (!port.to)
                        continue;

                    if (CheckCollisionPointLine(GetMousePosition(), port.Center(), port.to->Center(), 3))
                    {
                        printf("hovering wire"); // todo
                        break;
                    }
                }
            }
        }

        // Left click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (!hovering.PortEmpty())
            {
                if (!connectBuff.PortEmpty())
                {
                    if (!connectBuff.PortEmpty() && !hovering.PortEmpty())
                        Connect(connectBuff.GetPort(), hovering.GetPort());

                    connectBuff.Reset();
                }
                else
                {
                    connectBuff = hovering;
                }
            }
            else
            {
                Building* newBuild = new Building(buildingType);
                MoveBuilding(newBuild, GetMousePosition());
                buildings.push_back(newBuild);
                clickNumber++;
            }
        }
        // Right click
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) // Destroy building
        {
            if (hovering.building)
            {
                for (size_t i = 0; i < buildings.size(); ++i)
                {
                    if (buildings[i] == hovering.building)
                    {
                        for (Port& port : hovering.building->i)
                        {
                            Disconnect(&port);
                        }
                        for (Port& port : hovering.building->o)
                        {
                            Disconnect(&port);
                        }
                        buildings.erase(buildings.begin() + i);
                        delete hovering.building;
                        hovering.building = nullptr;
                        break;
                    }
                }
            }
        }
        // Hotkey
        if (int key = GetKeyPressed())
        {
            if (key >= KEY_ZERO && key <= KEY_NINE)
                buildingType = Building::Type(key - KEY_ZERO);
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            for (Building* building : buildings)
            {
                DrawBuilding(building);
            }
            DrawBuildingHighlighted(&hovering);
            if (!connectBuff.PortEmpty())
            {
                DrawBuildingHighlighted(&connectBuff);
                DrawLineV(connectBuff.GetPort()->Center(), GetMousePosition(), BLUE);
            }
            for (Building* building : buildings)
            {
                for (Port& port : building->o)
                {
                    DrawConnection(&port);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (Building* building : buildings)
    {
        delete building;
    }

    CloseWindow();

	return 0;
}