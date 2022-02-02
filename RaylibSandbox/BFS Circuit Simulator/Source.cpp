#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <fstream>
#include <stack>
#include <algorithm>
#include <unordered_map>

#include "Node.h"
#include "Component.h"
#include "Graph.h"

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

Vector2 ConstrainOffset(Vector2 constraint, Vector2 point)
{
    Vector2 difference = Vector2Subtract(point, constraint);
    float xSize = abs(difference.x);
    float ySize = abs(difference.y);
    auto [shortLength, longLength] = std::minmax(xSize, ySize);

    if (shortLength > longLength * 0.5) // Diagonal
    {
        return {
            constraint.x + copysignf(shortLength, difference.x),
            constraint.y + copysignf(shortLength, difference.y)
        };
    }
    else // Cardinal
    {
        if (xSize < ySize)
            return { constraint.x, point.y };
        else
            return { point.x, constraint.y };
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
    bool holdingTemporaryComponent = false; // If move is cancelled while this is true, we need to clean up the component

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
            cursor = ConstrainOffset(selectionStart->GetPosition(), cursor);

        // Hover
        Node* hoveredNode = graph.FindNodeAtGridPoint(cursor);
        Wire hoveredWire = (hoveredNode ? WireNull()  : graph.FindWireIntersectingGridPoint(cursor));

        if (!marqueeSelecting)
        {
            if (!selection.empty())
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (!holdingTemporaryComponent)
                        marqueeStart = cursor;
                    marqueeDragging = true;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    marqueeDragging = false;
                    Vector2 offset = cursor - marqueeStart;

                    // Copy
                    if (IsModifierDown(KEY_ALT) && !holdingTemporaryComponent)
                        graph.CloneNodesWithOffset(selection, offset);
                    // Move
                    else
                    {
                        graph.OffsetNodes(selection, offset);

                        // todo: ????
                        if (holdingTemporaryComponent)
                            holdingTemporaryComponent = false;
                    }
                }
            }
            else
            {
                // Drag
                if (selectionStart)
                {
                    selectionEnd = hoveredNode;

                    // Finish making connection
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        if (selectionEnd != selectionStart)
                        {
                            if (!selectionEnd)
                                selectionEnd = graph.AddNewNode(cursor);

                            graph.ConnectNodes(selectionStart, selectionEnd);
                            graphDirty = true;

                            // Divide wire
                            if (hoveredWire)
                            {
                                graph.DivideWire(hoveredWire, selectionEnd);
                                hoveredWire = WireNull();
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
                        selectionStart = graph.AddNewNode(cursor);
                        graphDirty = true;

                        // Divide wire
                        if (hoveredWire)
                        {
                            graph.DivideWire(hoveredWire, selectionStart);
                            hoveredWire = WireNull();
                        }
                    }
                }

                drawGateOptions = (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && selectionStart);
                if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && !selectionStart)
                {
                    if (hoveredNode)
                        selectionStart = hoveredNode;
                    else if (hoveredWire)
                    {
                        hoveredWire = graph.ReverseWire(hoveredWire);
                        graphDirty = true;
                    }
                }
                if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON) && selectionStart)
                {

                    // Middle click
                    if (Vector2Equals(cursor, selectionStart->GetPosition()))
                    {
                        selectionStart->SetGate(selectionStart->GetGate() != Gate::NOR ? Gate::NOR : Gate::OR);
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

                    selectionStart = selectionEnd = nullptr;
                }
            }
        }

        // Delete selected
        if ((IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) && !selection.empty())
        {
            for (Selectable element : selection)
            {
                if (element.type == Selectable::Type::COMP)
                    graph.RemoveComponentEntirely(element.comp);
                else
                    graph.RemoveNodeEntirely(element.node);
            }
            selection.clear();
        }

        // Make sure no other locally stored nodes can possibly exist at the time, or set them to nullptr
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !selectionStart && !selectionEnd && !marqueeSelecting)
        {
            if (hoveredNode)
            {
                bool removed;

                // Transfer inputs to outputs
                if (IsModifierDown(KEY_SHIFT) &&
                    !(hoveredNode->InputCount () == 1 &&
                      hoveredNode->OutputCount() == 1))
                {
                    hoveredNode->SetGate(Gate::OR);
                }
                else
                {
                    if (IsModifierDown(KEY_SHIFT))
                        graph.ConnectNodes(hoveredNode->GetInput(0), hoveredNode->GetOutput(0));

                    graph.RemoveNodeEntirely(hoveredNode);
                    hoveredNode = nullptr;
                }

                graphDirty = true;
            }
            else if (hoveredWire)
            {
                if (IsModifierDown(KEY_SHIFT)) // Also delete the related nodes
                {
                    graph.RemoveNodeEntirely(hoveredWire.a);
                    graph.RemoveNodeEntirely(hoveredWire.b);
                }
                else
                    graph.DisconnectNodes(hoveredWire.a, hoveredWire.b);

                hoveredWire = WireNull();
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

            ComponentBlueprint* blueprint = new ComponentBlueprint();
            blueprint->GenerateBlueprintFromSelection(breakdown);
            selection.clear();
            marqueeStart = { g_gridUnit * -6, 0 };
            Component* clone = graph.CloneComponentAtPosition(blueprint, marqueeStart); // Place the component offscreen
            selection.push_back(Selectable(clone));
            holdingTemporaryComponent = true;
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
            if (marqueeDragging || holdingTemporaryComponent)
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

    for (Node* node : graph.GetNodes())
    {
        delete node;
    }

    CloseWindow();

	return 0;
}