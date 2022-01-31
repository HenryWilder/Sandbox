#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>
#include <unordered_map>

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

#include "Node.h"
#include "Component.h"
#include "Graph.h"

float Vector2DistanceToLine(Vector2 startPos, Vector2 endPos, Vector2 point)
{
    Vector2 direction = Vector2Normalize(endPos - startPos);
    Vector2 nearestPoint = startPos + direction * Clamp(Vector2DotProduct(direction, point - startPos), 0, Vector2Distance(startPos, endPos));
    return Vector2Distance(point, nearestPoint);
}
bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center, float radius)
{
    return Vector2DistanceToLine(startPos, endPos, center) <= radius;
}

int Snap(int x, int gridsize)
{
    x /= gridsize;
    return x * gridsize;
}

Vector2 Snap(Vector2 pt, float gridsize)
{
    return {
        roundf(pt.x / gridsize) * gridsize,
        roundf(pt.y / gridsize) * gridsize
    };
}

float Signf(float value)
{
    return (value > 0.01f ? 1.0f : (value < -0.01f ? -1.0f : 0.0f));
}
void CompressWire(Node* start, Node* end, float length)
{
    Vector2 pos = end->GetPosition();
    pos.x = start->GetPosition().x + length * Signf(pos.x - start->GetPosition().x);
    pos.y = start->GetPosition().y + length * Signf(pos.y - start->GetPosition().y);
    end->SetPosition(pos);
}
void CompressNetwork(Node* start, float length)
{
    std::vector<Node*> wave;
    start->SetVisited(true);


    for (Node* output : start->GetOutputs())
    {
        if (!output->GetVisited())
            wave.push_back(output);
    }
    for (Node* input : start->GetInputs())
    {
        if (!input->GetVisited())
            wave.push_back(input);
    }

    for (Node* node : wave)
    {
        node->SetVisited(true);
        CompressWire(start, node, length);
    }
    for (Node* node : wave)
    {
        CompressNetwork(node, length);
    }
    wave.clear();
}

struct GateSelection
{
    static constexpr Gate gates[4] = {
        Gate::OR,
        Gate::AND,
        Gate::NOR,
        Gate::XOR,
    };
    static constexpr Vector2 sectors[_countof(gates)] =
    {
        Vector2{ 0, -1 },
        Vector2{ +1, 0 },
        Vector2{ 0, +1 },
        Vector2{ -1, 0 },
    };

    size_t index;
    bool b_toggle;
};

GateSelection ClosestHoveredGateOption(Vector2 cursor, Vector2 origin)
{
    if (Vector2Distance(cursor, origin) > 0.01f)
    {
        Vector2 direction = Vector2Normalize(cursor - origin);
        float cloesetDot = -1;
        size_t closestIndex = -1;
        for (size_t i = 0; i < 4; ++i)
        {
            float dot = Vector2DotProduct(direction, Vector2Normalize(GateSelection::sectors[i]));
            if (dot > cloesetDot)
            {
                cloesetDot = dot;
                closestIndex = i;
            }
        }
        if (cloesetDot >= 0.5f)
            return { closestIndex, false };
    }
    return { 0, true };
}

enum ModifierKey
{
    KEY_CONTROL,
    KEY_ALT,
    KEY_SHIFT,
};
bool IsModifierDown(int id)
{
    switch (id)
    {
    case KEY_CONTROL: return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    case KEY_ALT: return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    case KEY_SHIFT: return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    default: return false;
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

    Graph graph;
    bool graphDirty = false;
    bool gateDirty = false;
    bool drawGateOptions = false;

    Node* selectionStart = nullptr;
    Node* selectionEnd = nullptr;

    bool marqueeSelecting = false;
    bool marqueeDragging = false;
    Vector2 marqueeStart = Vector2Zero();
    Rectangle selectionRec;
    std::vector<Selectable> selection;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Saving and loading
        if (IsKeyPressed(KEY_S) && IsModifierDown(KEY_CONTROL))
        {
            if (IsModifierDown(KEY_SHIFT)) // Load
                graph.Load("save.graph");
            else // Save
                graph.Save("save.graph");
        }


        // Grid-snapped cursor
        Vector2 cursor = Snap(GetMousePosition(), g_nodeRadius * 2.0f);

        // Lock wire directions
        if (selectionStart)
        {
            Vector2 start = selectionStart->GetPosition();
            Vector2 difference = Vector2Subtract(cursor, start);
            float xSize = abs(difference.x);
            float ySize = abs(difference.y);
            auto[shortLength, longLength] = std::minmax(xSize, ySize);

            if (shortLength > longLength * 0.5) // Diagonal
            {
                cursor = {
                    start.x + copysignf(shortLength, difference.x),
                    start.y + copysignf(shortLength, difference.y)
                };
            }
            else // Cardinal
            {
                if (xSize < ySize)
                    cursor.x = start.x;
                else
                    cursor.y = start.y;
            }
        }

        // Hover
        Node* hoveredNode = graph.FindNodeAtPosition(cursor, g_nodeRadius * 2.0f);
        Wire hoveredWire = (hoveredNode ? Wire{ nullptr, nullptr }  : graph.FindWireIntersectingPosition(cursor, g_nodeRadius * 2.0f));

        if (!marqueeSelecting)
        {
            if (!selection.empty())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    marqueeStart = cursor;
                    marqueeDragging = true;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    marqueeDragging = false;
                    Vector2 offset = cursor - marqueeStart;

                    // Copy
                    if (IsModifierDown(KEY_ALT))
                    {
                        std::vector<Selectable> copies;

                        copies.reserve(selection.size());
                        for (Selectable src : selection)
                        {
                            if (src.type == Selectable::Type::COMP)
                            {
                                graph.CloneComponentAtPosition(src.comp->GetBlueprint(), src.comp->GetPosition() + offset);
                            }
                            else
                            {
                                Selectable copy = Selectable(new Node(src.node->GetPosition() + offset, src.node->GetGate()));
                                copies.push_back(copy);
                                graph.AddNode(copies.back().node);
                            }
                        }

                        graph.ResetVisited();
                        for (Selectable start : selection)
                        {
                            if (start.node->GetVisited())
                                continue;

                            size_t a = std::find(selection.begin(), selection.end(), start) - selection.begin();

                            for (Node* end : start.node->GetOutputs())
                            {
                                if (end->GetVisited())
                                    break;

                                auto it = std::find(selection.begin(), selection.end(), end);

                                if (it != selection.end())
                                {
                                    size_t b = it - selection.begin();
                                    graph.ConnectNodes(copies[a].node, copies[b].node);
                                }
                            }
                        }
                    }
                    // Move
                    else
                    {
                        for (Selectable element : selection)
                        {
                            switch (element.type)
                            {
                            case Selectable::Type::NODE:
                                element.node->SetPosition(element.node->GetPosition() + offset);
                                break;
                            case Selectable::Type::COMP:
                                element.comp->SetPosition(element.comp->GetPosition() + offset);
                            }
                        }
                    }
                }
            }
            else
            {
                // Drag
                if (selectionStart)
                {
                    selectionEnd = hoveredNode;

                    // Finalize
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        if (selectionEnd != selectionStart)
                        {
                            if (!selectionEnd)
                            {
                                selectionEnd = new Node(cursor, Gate::OR);
                                graph.AddNode(selectionEnd);
                            }
                            graph.ConnectNodes(selectionStart, selectionEnd);
                            graphDirty = true;

                            // Divide wire
                            if (hoveredWire.a)
                            {
                                graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                                graph.ConnectNodes(hoveredWire.a, selectionEnd);
                                graph.ConnectNodes(selectionEnd, hoveredWire.b);
                            }
                        }
                        selectionStart = selectionEnd = nullptr;
                    }
                }
                // Create new node
                else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !selectionStart)
                {
                    selectionStart = hoveredNode;

                    if (!selectionStart)
                    {
                        selectionStart = new Node(cursor, Gate::OR);
                        hoveredNode = selectionStart;
                        graph.AddNode(selectionStart);
                        graphDirty = true;

                        // Divide wire
                        if (hoveredWire.a)
                        {
                            graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                            graph.ConnectNodes(hoveredWire.a, selectionStart);
                            graph.ConnectNodes(selectionStart, hoveredWire.b);
                        }
                    }
                }

                drawGateOptions = (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && selectionStart);
                if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && !selectionStart)
                {
                    if (hoveredNode)
                    {
                        selectionStart = hoveredNode;
                    }
                    else if (hoveredWire.a)
                    {
                        graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                        graph.ConnectNodes(hoveredWire.b, hoveredWire.a);
                        hoveredWire = { hoveredWire.b, hoveredWire.a };
                        graphDirty = true;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON) && selectionStart)
                {

                    // Middle click
                    if (cursor.x == selectionStart->GetPosition().x &&
                        cursor.y == selectionStart->GetPosition().y)
                    {
                        if (selectionStart->GetGate() != Gate::NOR)
                            selectionStart->SetGate(Gate::NOR);
                        else
                            selectionStart->SetGate(Gate::OR);
                    }
                    // Middle drag
                    else
                    {
                        // In clockwise order
                        GateSelection select = ClosestHoveredGateOption(cursor, selectionStart->GetPosition());
                        selectionStart->SetGate(GateSelection::gates[select.index]);
                    }

                    // Inputless nodes take output from the user and therefore are treated as mutable
                    if (!selectionStart->HasNoInputs())
                        gateDirty = true;
                    else
                        selectionStart->SetState(selectionStart->GetGate() == Gate::NOR);

                    selectionStart = nullptr;
                    selectionEnd = nullptr;
                }
            }
        }

        // Delete selected
        if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty())
        {
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    graph.RemoveComponent(element.comp);
                    delete element.comp;
                }
                else
                {
                    graph.RemoveNode(element.node);
                    delete element.node;
                }
            }
            selection.clear();
        }

        // Make sure no other locally stored nodes can possibly exist at the time, or set them to nullptr
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !selectionStart && !selectionEnd && !marqueeSelecting)
        {
            if (hoveredNode)
            {
                graph.RemoveNode(hoveredNode);
                delete hoveredNode;
                hoveredNode = nullptr;
                graphDirty = true;
            }
            else if (hoveredWire.a)
            {
                graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);
                hoveredWire = { nullptr, nullptr };
                graphDirty = true;
            }
            else
            {
                marqueeStart = cursor;
                marqueeSelecting = true;
            }
        }
        if (marqueeSelecting)
        {
            Vector2 a = {
                std::min(marqueeStart.x, cursor.x),
                std::min(marqueeStart.y, cursor.y)
            };
            selectionRec = {
                    a.x,
                    a.y,
                    std::max(marqueeStart.x, cursor.x) - a.x,
                    std::max(marqueeStart.y, cursor.y) - a.y,
            };
            graph.FindSelectablesInRectangle(&selection, selectionRec);

            // Finalize
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
                marqueeSelecting = false;
        }

        // Package up components
        if (IsKeyPressed(KEY_SPACE) && selection.size() > 1)
        {
            size_t nodeCount = selection.size();
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    nodeCount += element.comp->GetNodeCount() - 1;
                }
            }
            std::vector<Node*> breakdown;
            breakdown.reserve(nodeCount);
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    for (Node* node : element.comp->GetNodes())
                    {
                        breakdown.push_back(node);
                    }
                }
                else
                {
                    breakdown.push_back(element.node);
                }
            }

            ComponentBlueprint* blueprint = new ComponentBlueprint(breakdown);

            Vector2 minPoint = { INFINITY, INFINITY };
            for (Selectable element : selection)
            {
                Vector2 pos;

                if (element.type == Selectable::Type::COMP)
                    pos = element.comp->GetPosition();
                else
                    pos = element.node->GetPosition();

                if (pos.x < minPoint.x)
                    minPoint.x = pos.x;

                if (pos.y < minPoint.y)
                    minPoint.y = pos.y;
            }
            graph.CloneComponentAtPosition(blueprint, minPoint);

            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    graph.RemoveComponent(element.comp);
                    delete element.comp;
                }
                else
                {
                    graph.RemoveNode(element.node);
                    delete element.node;
                }
            }
            selection.clear();

            graphDirty = true;
        }

        graph.UnDirty();
        graph.Step();

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(BLACK);

            constexpr int gridRadius = 8;

            // World grid
            static const Color worldGridColor = ColorAlpha(DARKGRAY, 0.125f);
            for (float x = 0.0f; x < (float)windowWidth; x += 2.0f * g_nodeRadius)
            {
                DrawLineV({ x, 0 }, { x, (float)windowHeight }, worldGridColor);
            }
            for (float y = 0.0f; y < (float)windowHeight; y += 2.0f * g_nodeRadius)
            {
                DrawLineV({ 0.0f, y }, { (float)windowWidth, y }, worldGridColor);
            }

            if (marqueeSelecting)
            {
                DrawRectangleRec(selectionRec, ColorAlpha(GRAY, 0.125));
                DrawRectangleLines((int)lroundf(selectionRec.x), (int)lroundf(selectionRec.y), (int)lroundf(selectionRec.width), (int)lroundf(selectionRec.height), GRAY);
            }


            // Current wire being made
            if (selectionStart)
            {
                Vector2 end;
                if (selectionEnd)
                    end = selectionEnd->GetPosition();
                else
                    end = cursor;

                DrawLineV(selectionStart->GetPosition(), end, GRAY);

                // Cursor grid
                Vector2 p = selectionStart->GetPosition();
                Vector2 c = cursor;

                float xLength = abs(c.x - p.x);
                float yLength = abs(c.y - p.y);
                float s = xLength;

                Vector2 directions[3];
                directions[0] = { (c.x < p.x ? -1.0f : 1.0f), (c.y < p.y ? -1.0f : 1.0f) };
                if (xLength == yLength)
                {
                    directions[1] = { directions[0].x, 0.0f };
                    directions[2] = { 0.0f, directions[0].y };
                }
                else
                {
                    if (xLength > yLength)
                    {
                        directions[1] = { directions[0].x, -directions[0].x };
                        directions[2] = { directions[0].x, +directions[0].x };
                        directions[0].y = 0.0f;
                    }
                    else
                    {
                        s = yLength;
                        directions[1] = { -directions[0].y, directions[0].y };
                        directions[2] = { +directions[0].y, directions[0].y };
                        directions[0].x = 0.0f;
                    }
                }

                for (int o = -gridRadius; o <= gridRadius; ++o)
                {
                    Color color = ColorAlpha(WHITE, 1.0f - abs((float)o / (float)gridRadius));
                    for (int i = 0; i < 3; ++i)
                    {
                        DrawPixelV((p + directions[i] * s) + (directions[i] * g_gridUnit * (float)o), color);
                    }
                }

                for (int i = -gridRadius; i <= gridRadius; ++i)
                {
                    float offset = i * g_gridUnit - 1;
                    Color color = ColorAlpha(WHITE, 1.0f - Clamp(abs(i) / (float)gridRadius, 0.0f, 1.0f));
                    Vector2 offsets[2] = {
                        { 0.0f, offset },
                        { offset, 0.0f }
                    };
                    for (int i = 0; i < 2; ++i)
                    {
                        DrawPixelV(cursor + offsets[i], color);
                    }
                }
            }
            else
            {
                // Cursor grid
                for (int y = -gridRadius; y <= gridRadius; ++y)
                {
                    int halfWidth = (int)lroundf(sqrtf((float)(gridRadius * gridRadius - y * y)));
                    for (int x = -halfWidth; x <= halfWidth; ++x)
                    {
                        Vector2 offset;
                        offset.x = x * g_gridUnit;
                        offset.y = y * g_gridUnit;
                        float alpha = 1.0f - Clamp(Vector2Length(offset) / ((float)gridRadius * g_gridUnit), 0.0f, 1.0f);
                        DrawPixelV(cursor + offset, ColorAlpha(WHITE, alpha));
                    }
                }
            }

            // Crosshair
            {
                Color color = ColorAlpha(WHITE, 0.0625);
                DrawLineV({ cursor.x, 0 }, { cursor.x, (float)windowHeight }, color);
                DrawLineV({ 0, cursor.y }, { (float)windowWidth,  cursor.y }, color);
            }


            // Wires
            graph.DrawWires();

            // Moving points
            if (marqueeDragging)
            {
                Vector2 offset = cursor - marqueeStart;
                for (Selectable element : selection)
                {
                    if (element.type == Selectable::Type::COMP)
                    {
                        /*
                        * 0---1
                        * | 3-2
                        * 5-4
                        * 
                        * 0 is repeated for 6
                        */
                        const Rectangle& a = element.comp->GetCasingA();
                        const Rectangle& b = element.comp->GetCasingB();

                        Vector2 points[7] = {
                            { a.x + offset.x,           a.y + offset.y            },
                            { b.x + offset.x + b.width, a.y + offset.y            },
                            { b.x + offset.x + b.width, b.y + offset.y + b.height },
                            { b.x + offset.x,           b.y + offset.y + b.height },
                            { b.x + offset.x,           a.y + offset.y + a.height },
                            { a.x + offset.x,           a.y + offset.y + a.height },
                            points[0]
                        };

                        DrawLineStrip(points, 7, MAGENTA);
                    }
                    else
                        DrawCircleV(element.node->GetPosition() + offset, 2.0f, MAGENTA);
                }
            }

            // Selection
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                {
                    Rectangle rec;

                    rec = element.comp->GetCasingA();
                    rec.x -= 2.0f;
                    rec.y -= 2.0f;
                    rec.width += 4.0f;
                    rec.height += 4.0f;
                    DrawRectangleRec(rec, WHITE);

                    rec = element.comp->GetCasingB();
                    rec.x -= 2.0f;
                    rec.y -= 2.0f;
                    rec.width += 4.0f;
                    rec.height += 4.0f;
                    DrawRectangleRec(rec, WHITE);
                }
                else
                    DrawGateIcon(element.node->GetGate(), element.node->GetPosition(), g_nodeRadius + 2.0f, WHITE);
            }

            // Components
            graph.DrawComponents();

            // Hovered wire
            if (hoveredWire.a && !drawGateOptions)
                DrawLineV(hoveredWire.a->GetPosition(), hoveredWire.b->GetPosition(), LIGHTGRAY);

            // Nodes
            graph.DrawNodes();

            // Cursor
            if (hoveredNode)
                DrawHighlightedGate(hoveredNode->GetPosition(), hoveredNode->GetGate());
            else
                DrawRectanglePro({ cursor.x,cursor.y,5,5 }, { 2.5,2.5 }, 45, RAYWHITE);

            // Gate options
            if (drawGateOptions)
            {
                if (selectionStart)
                    DrawHighlightedGate(selectionStart->GetPosition(), selectionStart->GetGate());

                GateSelection select = ClosestHoveredGateOption(cursor, selectionStart->GetPosition());
                Gate pick;
                if (select.b_toggle)
                    pick = (selectionStart->GetGate() != Gate::NOR ? Gate::NOR : Gate::OR);
                else
                    pick = GateSelection::gates[select.index];

                for (size_t i = 0; i < 4; ++i)
                {
                    constexpr float l = g_nodeRadius * 7.0f;
                    Vector2 center = GateSelection::sectors[i] * l;
                    DrawCircleV(selectionStart->GetPosition() + center, g_nodeRadius * 3.0f + 2.0f, GRAY);
                    DrawCircleV(selectionStart->GetPosition() + center, g_nodeRadius * 3.0f, (pick == GateSelection::gates[i] ? GRAY : Color{ 30, 30, 30, 255 }));
                    DrawGateIcon(GateSelection::gates[i], selectionStart->GetPosition() + center, g_nodeRadius * 1.5f, WHITE);
                }
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    for (Node* node : graph.nodes)
    {
        delete node;
    }

    CloseWindow();

	return 0;
}