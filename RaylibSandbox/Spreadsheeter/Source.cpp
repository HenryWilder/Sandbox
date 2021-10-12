#include <string>
#include <vector>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

int Min(int a, int b)
{
    if (a > b)
        return b;
    else
        return a;
}
int Max(int a, int b)
{

    if (a < b)
        return b;
    else
        return a;
}

constexpr Color ColorFromHex(unsigned int value)
{
    return {
        (unsigned char)(value >> 020),
        (unsigned char)(value >> 010),
        (unsigned char)(value),
        (unsigned char)(255)
    };
}

constexpr Color g_palette[] =
{
    // Grayscale
    ColorFromHex(0x000000), // Black
    ColorFromHex(0x434343), // Dark gray 4
    ColorFromHex(0x666666), // Dark gray 3
    ColorFromHex(0x999999), // Dark gray 2
    ColorFromHex(0xb7b7b7), // Dark gray 1
    ColorFromHex(0xcccccc), // Gray
    ColorFromHex(0xd9d9d9), // Light gray 1
    ColorFromHex(0xefefef), // Light gray 2
    ColorFromHex(0xf3f3f3), // Light gray 3
    ColorFromHex(0xffffff), // White

    // Bright
    ColorFromHex(0x980000), // Red Berry
    ColorFromHex(0xff0000), // Red
    ColorFromHex(0xff9900), // Orange
    ColorFromHex(0xffff00), // Yellow
    ColorFromHex(0x00ff00), // Green
    ColorFromHex(0x00ffff), // Cyan
    ColorFromHex(0x4a86e8), // Cornflower Blue
    ColorFromHex(0x0000ff), // Blue
    ColorFromHex(0x9900ff), // Purple
    ColorFromHex(0xff00ff), // Magenta
};

#define BLACK       g_palette[0]
#define DARKGRAY4   g_palette[1]
#define DARKGRAY3   g_palette[2]
#define DARKGRAY2   g_palette[3]
#define DARKGRAY1   g_palette[4]
#define GRAY        g_palette[5]
#define LIGHTGRAY1  g_palette[6]
#define LIGHTGRAY2  g_palette[7]
#define LIGHTGRAY3  g_palette[8]
#define WHITE       g_palette[9]

#define REDBERRY        g_palette[10]
#define RED             g_palette[11]
#define ORANGE          g_palette[12]
#define YELLOW          g_palette[13]
#define GREEN           g_palette[14]
#define CYAN            g_palette[15]
#define CORNFLOWER_BLUE g_palette[16]
#define PURPLE          g_palette[17]
#define MAGENTA         g_palette[18]

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

std::vector<int> g_ColumnWidths = { 120, 220, 320, 420, 520, 620, 720, 820, };
std::vector<int> g_RowHeights = { 41, 62, 83, 104, 125, 146, 167, 188, 209, 230, 251, 272, 293, 314, 335, 356, };

struct Cell
{
    int x, y;
};
struct Range
{
    Cell start, end;
};
std::vector<Range> g_selection = {};

int PreviousRowHeight(int row)
{
    if (row > 0)
        return g_RowHeights[(size_t)row - 1];
    else
        return 20;
}
int CellHeight(int row)
{
    return g_RowHeights[row] - PreviousRowHeight(row);
}
int PreviousColumnWidth(int column)
{
    if (column > 0)
        return g_ColumnWidths[(size_t)column - 1];
    else
        return 20;
}
int CellWidth(int column)
{
    return g_ColumnWidths[column] - PreviousColumnWidth(column);
}

void DrawResizeBar(int x, int y, Color color)
{
    if (x > 0) // Vertical
    {
        DrawRectangle(x - 3, y, 5, 21, color);
        DrawLine(x, 21, x, g_RowHeights.back(), color);
    }
    else // Horizontal
    {
        DrawRectangle(x, y - 2, 20, 5, color);
        DrawLine(20, y, g_ColumnWidths.back(), y, color);
    }
}
void HighlightRange(Range range, Color color)
{
    int startX = PreviousColumnWidth(range.start.x);
    int startY = PreviousRowHeight(range.start.y);
    DrawRectangle(startX, startY, g_ColumnWidths[range.end.x] - startX, g_RowHeights[range.end.y] - startY, color);
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Spreadsheeter");
    SetTargetFPS(60);

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    enum class MouseMode
    {
        None,
        Hovering,
        Dragging,
    } mouseMode = MouseMode::None;

    enum class Region
    {
        Outside,
        Cell,
        Edge,
    } region = Region::Outside;

    // -1 for none
    int cell_x = -1;
    int cell_y = -1;
    int drag_offset = 0;
    int drag_distance = 0;

    Cell startOfSelection;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Something must already be selected before selection can be eliminated
        if (mouseMode == MouseMode::Dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            // Column
            if (cell_x != -1)
            {
                for (int i = cell_x; i < g_ColumnWidths.size(); ++i)
                {
                    g_ColumnWidths[i] += drag_distance;
                }
            }
            // Row
            else
            {
                for (int i = cell_y; i < g_RowHeights.size(); ++i)
                {
                    g_RowHeights[i] += drag_distance;
                }
            }
            mouseMode = MouseMode::None;
        }

        // Skip checking if we already know they're dragging something
        if (mouseMode != MouseMode::Dragging)
        {
            region = Region::Outside;
            cell_x = cell_y = -1;

            // Find column
            if (GetMouseX() > 20)
            {
                for (int i = 0; i < g_ColumnWidths.size(); ++i)
                {
                    if (GetMouseX() <= g_ColumnWidths[i])
                    {
                        cell_x = i;
                        if (GetMouseY() <= 20 && (GetMouseX() + 5) >= g_ColumnWidths[i])
                        {
                            region = Region::Edge;
                            drag_offset = GetMouseX() - g_ColumnWidths[i];
                        }
                        break;
                    }
                }
            }

            // Find row
            if (GetMouseY() > 20)
            {
                for (int i = 0; i < g_RowHeights.size(); ++i)
                {
                    if (GetMouseY() <= g_RowHeights[i])
                    {
                        cell_y = i;
                        if (GetMouseX() <= 20 && (GetMouseY() + 5) >= g_RowHeights[i])
                        {
                            region = Region::Edge;
                            drag_offset = GetMouseY() - g_RowHeights[i];
                        }
                        break;
                    }
                }
            }

            if (cell_x >= 0 && cell_y >= 0)
                region = Region::Cell;

            if (region == Region::Edge)
            {
                if (cell_x != -1)
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                else
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
            }
            else
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
                    g_selection.clear();

                if (region == Region::Edge)
                    mouseMode = MouseMode::Dragging;

                else if (region == Region::Cell)
                {
                    Cell cell = { cell_x, cell_y };

                    if (!g_selection.empty() && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
                    {
                        Range& range = g_selection.back();
                        range.start.x = Min(cell_x, startOfSelection.x);
                        range.end.x = Max(cell_x, startOfSelection.x);
                        range.start.y = Min(cell_y, startOfSelection.y);
                        range.end.y = Max(cell_y, startOfSelection.y);
                    }
                    else
                    {
                        startOfSelection = cell;

                        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
                        {
                            g_selection.push_back({ cell, cell }); // todo
                        }
                        else
                            g_selection.push_back({ cell, cell });
                    }
                }
            }
        }

        if (mouseMode == MouseMode::Dragging)
        {
            if (region == Region::Edge)
            {
                if (cell_x != -1) // Vertical
                {
                    drag_distance = GetMouseX() - g_ColumnWidths[cell_x];
                    // Clamp
                    if (drag_distance < 0 && (g_ColumnWidths[cell_x] + drag_distance) < (PreviousColumnWidth(cell_x) + 6))
                        drag_distance = PreviousColumnWidth(cell_x) - g_ColumnWidths[cell_x] + 6;
                    
                }
                else // Horizontal
                {
                    drag_distance = GetMouseY() - g_RowHeights[cell_y];
                    // Clamp
                    if (drag_distance < 0 && (g_RowHeights[cell_y] + drag_distance) < (PreviousRowHeight(cell_y) + 6))
                        drag_distance = PreviousRowHeight(cell_y) - g_RowHeights[cell_y] + 6;
                    
                }
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(WHITE);

            for (size_t i = 0; i < g_selection.size(); ++i)
            {
                HighlightRange(g_selection[i], CORNFLOWER_BLUE);
            }

            DrawRectangle(0, 0, 20, g_RowHeights.back(), RAYWHITE);
            DrawRectangle(0, 0, g_ColumnWidths.back(), 20, RAYWHITE);
            DrawLine(20, 0, 20, g_RowHeights.back(), DARKGRAY1);
            DrawLine(0, 20, g_ColumnWidths.back(), 20, DARKGRAY1);
            for (int i = 0; i < g_ColumnWidths.size(); ++i)
            {
                DrawLine(g_ColumnWidths[i], 0, g_ColumnWidths[i], 20, DARKGRAY1);
                DrawLine(g_ColumnWidths[i], 21, g_ColumnWidths[i], g_RowHeights.back(), LIGHTGRAY1);
                DrawText((i > 26 ? TextFormat("%c%c", 'A' + i / 26, 'A' + i % 26) : TextFormat("%c",'A' + i)), (g_ColumnWidths[i] + PreviousColumnWidth(i)) / 2 - 3.5f, 5, 10, DARKGRAY3);
            }
            for (int i = 0; i < g_RowHeights.size(); ++i)
            {
                DrawLine(0, g_RowHeights[i], 20, g_RowHeights[i], DARKGRAY1);
                DrawLine(20, g_RowHeights[i], g_ColumnWidths.back(), g_RowHeights[i], LIGHTGRAY1);
                DrawText(TextFormat("%i", i), 10 - 3.5f, (g_RowHeights[i] + PreviousRowHeight(i)) / 2 - 2.5f, 10, DARKGRAY3);
            }

            if (region == Region::Edge)
            {
                if (cell_x != -1)
                {
                    if (mouseMode == MouseMode::Dragging)
                        DrawResizeBar(g_ColumnWidths[cell_x] + drag_distance, 0, CORNFLOWER_BLUE);
                    else
                        DrawRectangle(g_ColumnWidths[cell_x] - 5, 0, 5, 21, CORNFLOWER_BLUE);
                }
                else if (cell_y != -1)
                {
                    if (mouseMode == MouseMode::Dragging)
                        DrawResizeBar(0, g_RowHeights[cell_y] + drag_distance, CORNFLOWER_BLUE);
                    else
                        DrawRectangle(0, g_RowHeights[cell_y] - 4, 20, 5, CORNFLOWER_BLUE);
                }
            }
            //else if (region == Region::Cell)
            //{
            //    DrawRectangle(PreviousColumnWidth(cell_x), PreviousRowHeight(cell_y), CellWidth(cell_x), CellHeight(cell_y), CORNFLOWER_BLUE);
            //}

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}