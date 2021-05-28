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

struct Building;
struct Port;
struct Port
{
    static constexpr int s_width = 16;
    static constexpr int s_length = 4;

    Building* of; // Back pointer
    Port* to = nullptr; // Simply a connector, not at all owned by this object
    Rectangle shape = Rectangle{};
};
struct Building
{
    enum Type
    {
        miner,
        storage,
        splitter,
        merger,
        constructor,
        assembler,
        manufacturer,
        refinery,

        COUNT
    };
    Building(Type type)
    {
        switch (type)
        {
        case Building::miner:
            o.push_back(new Port);
            inv.resize(1);
            break;
        case Building::storage:
            i.push_back(new Port);
            o.push_back(new Port);
            inv.resize(24);
            break;
        case Building::splitter:
            i.push_back(new Port);
            o.reserve(3);
            o.push_back(new Port);
            o.push_back(new Port);
            o.push_back(new Port);
            inv.resize(1);
            break;
        case Building::merger:
            i.reserve(3);
            i.push_back(new Port);
            i.push_back(new Port);
            i.push_back(new Port);
            o.push_back(new Port);
            inv.resize(1);
            break;
        case Building::constructor:
            i.push_back(new Port);
            o.push_back(new Port);
            inv.resize(2);
            break;
        case Building::assembler:
            i.reserve(2);
            i.push_back(new Port);
            i.push_back(new Port);
            o.push_back(new Port);
            inv.resize(3);
            break;
        case Building::manufacturer:
            i.reserve(4);
            i.push_back(new Port);
            i.push_back(new Port);
            i.push_back(new Port);
            i.push_back(new Port);
            o.push_back(new Port);
            inv.resize(5);
            break;
        case Building::refinery:
            i.reserve(2);
            i.push_back(new Port);
            i.push_back(new Port);
            o.reserve(2);
            o.push_back(new Port);
            o.push_back(new Port);
            inv.resize(4);
            break;
        }
        for (Port* port : i)
        {
            port->of = this;
        }
        for (Port* port : o)
        {
            port->of = this;
        }
    }

    static constexpr int s_padding = 4;
    static constexpr int s_length = 32;

    std::vector<Port*> i; // Created and owned by this object
    std::vector<Port*> o; // Created and owned by this object
    std::vector<ItemSlot> inv;
    Rectangle shape = Rectangle{};
};


// Returns true on success (will not connect if connections are full)
// src must be on output side, dest must be on input side
bool Connect(Building* src, int portID_src, Building* dest, int portID_dest)
{
    if ((src && dest) &&
        ((src->o.size() > portID_src) && (dest->i.size() > portID_dest)) &&
        (src->o[portID_src] && dest->i[portID_dest]) &&
        (!src->o[portID_src]->to && !dest->i[portID_dest]->to))
    {
        src->o[portID_src]->to = dest->i[portID_dest];
        dest->i[portID_dest]->to = src->o[portID_src];
        return true;
    }
    else
        return false;
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
        building->i[i]->shape = PortRectangleAtPosition(building->shape.x + PadPort(i + (!inputHeavy * sidesDifference)) + Port::s_width / 2, building->shape.y);
    }
    for (int o = 0; o < building->o.size(); ++o)
    {
        building->o[o]->shape = PortRectangleAtPosition(building->shape.x + PadPort(o + (inputHeavy * sidesDifference)) + Port::s_width / 2, building->shape.y + Building::s_length);
    }
}
void DrawBuilding(Building* building)
{
    DrawRectangleRec(building->shape, DARKGRAY);

    for (Port* i : building->i)
    {
        DrawRectangleRec(i->shape, ORANGE);
    }
    for (Port* o : building->o)
    {
        DrawRectangleRec(o->shape, LIME);
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
    enum class Side { null, input, output } side;
    int port;

    Port* GetPort()
    {
        switch (side)
        {
        case Hover::Side::null:
            return nullptr;
        case Hover::Side::input:
            return building->i[port];
        case Hover::Side::output:
            return building->o[port];
        }
    }
};

void DrawBuildingHighlighted(Hover* hover)
{
    if (hover->building)
    {
        DrawRectangleRec(hover->building->shape, GRAY);

        for (Port* i : hover->building->i)
        {
            DrawRectangleRec(i->shape, ORANGE);
        }
        for (Port* o : hover->building->o)
        {
            DrawRectangleRec(o->shape, LIME);
        }
        if (hover->side == Hover::Side::input)
            DrawRectangleRec(hover->building->i[hover->port]->shape, YELLOW);
        else if (hover->side == Hover::Side::output)
            DrawRectangleRec(hover->building->o[hover->port]->shape, GREEN);
    }
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
    Hover start, end;

    enum class State
    {
        normal,
        select_start,
        select_end,
        dragging,
    } state = State::normal;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        hovering = { nullptr, Hover::Side::null, -1 };
        for (Building* building : buildings)
        {
            if (CheckCollisionPointRec(GetMousePosition(), Rectangle{ building->shape.x, building->shape.y - Port::s_length / 2, building->shape.width, building->shape.height + Port::s_length }))
            {
                hovering.building = building;

                for (int i = 0; i < building->i.size(); ++i)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), building->i[i]->shape))
                    {
                        hovering.port = i;
                        hovering.side = Hover::Side::input;
                        break;
                    }
                }
                if (hovering.side != Hover::Side::null)
                    break;

                for (int i = 0; i < building->o.size(); ++i)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), building->o[i]->shape))
                    {
                        hovering.port = i;
                        hovering.side = Hover::Side::output;
                        break;
                    }
                }
                break;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (hovering.side != Hover::Side::null)
            {
                switch (state)
                {
                case State::normal:         state = State::select_start; break;
                case State::select_start:   state = State::select_end;   break;
                case State::select_end:     state = State::normal;       break;
                }
            }
            else
            {
                if (hovering.building)
                    state = State::dragging;
                else
                    state = State::normal;
            }

            switch (state)
            {
            case State::normal: {
                Building* newBuild = new Building(Building::Type(clickNumber % Building::COUNT));
                MoveBuilding(newBuild, GetMousePosition());
                buildings.push_back(newBuild);
                clickNumber++;
                }
                break;
            case State::select_start:
                if (hovering.side == Hover::Side::input)
                    end = hovering;
                else if (hovering.side == Hover::Side::output)
                    start = hovering;
                break;
            case State::select_end:
                if (hovering.side == Hover::Side::input)
                    end = hovering;
                else if (hovering.side == Hover::Side::output)
                    start = hovering;

                if (start.side != Hover::Side::null && end.side != Hover::Side::null)
                {
                    Connect(start.building, start.port, end.building, end.port);
                }
                start.side = end.side = Hover::Side::null;
                state = State::normal;
                break;
            case State::dragging:
                // todo
                break;
            }
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state == State::dragging)
        {
            state = State::normal;
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
            if (state == State::select_start)
            {
                DrawBuildingHighlighted(&start);
                Port* port = start.GetPort();
                DrawLineV({ port->shape.x + port->s_width / 2, port->shape.y + port->s_length / 2 }, GetMousePosition(), BLUE);
            }
            for (Building* building : buildings)
            {
                for (Port* port : building->o)
                {
                    DrawConnection(port);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (Building* building : buildings)
    {
        for (Port* port : building->i)
        {
            delete port;
        }
        for (Port* port : building->o)
        {
            delete port;
        }
        delete building;
    }

    CloseWindow();

	return 0;
}