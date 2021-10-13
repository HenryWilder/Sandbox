#include <string>
#include <vector>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

int Min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
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

#define SELECTION_BLUE { 0x4a, 0x86, 0xe8, 32 }

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

std::vector<int> g_ColumnWidths;
std::vector<int> g_RowHeights;

struct Cell
{
    int x, y;
};
struct Range
{
    Cell start, end;
};
// Does not say whether the rows/columns *match*
bool IsRangeContiguous(Range a, Range b)
{
    return
        a.end.x + 1 == b.start.x ||
        b.end.x + 1 == a.start.x ||
        a.end.y + 1 == b.start.y ||
        b.end.y + 1 == a.start.y;
}
bool IsRangeMatchingR(Range a, Range b)
{
    return a.start.y == b.start.y && b.end.y == a.end.y;
}
bool IsRangeMatchingC(Range a, Range b)
{
    return a.start.x == b.start.x && b.end.x == a.end.x;
}
bool IsRangeOverlapping(Range a, Range b)
{
    Rectangle ra = { a.start.x, a.start.y, a.end.x - a.start.x, a.end.y - a.start.y };
    Rectangle rb = { b.start.x, b.start.y, b.end.x - b.start.x, b.end.y - b.start.y };
    return CheckCollisionRecs(ra, rb);
}
Range RangeFromCellPair(Cell a, Cell b)
{
    return
        Range{
            Cell{
                Min(a.x, b.x),
                Min(a.y, b.y)
            },
            Cell{
                Max(a.x, b.x),
                Max(a.y, b.y)
            }
        };
}
void SimplifyMultiRange(std::vector<Range>* ranges)
{
    /*******************************************************************************************************
    *
    *   RANGE COMBINATION
    *
    *   Compatible:
    *
    *     0. ROWS match and COLUMNS are contiguous. Order does not matter.
    *
    *         [A][A][B][B] or [B][B][A][A]
    *         [A][A][B][B]    [B][B][A][A]
    *
    *         Result: Ranges can combine.
    *
    *         [A][A][A][A]
    *         [A][A][A][A]
    *
    *
    *     0. COLUMNS match and ROWS are contiguous. Order does not matter.
    *
    *         [A][A]    [B][B]
    *         [A][A] or [B][B]
    *         [B][B]    [A][A]
    *         [B][B]    [A][A]
    *
    *         Result: Ranges can combine.
    *
    *         [A][A]
    *         [A][A]
    *         [A][A]
    *         [A][A]
    *
    *
    *   Incompatible:
    *
    *     1. Ranges are not contiguous either by row nor by column, whether their rows or columns match
    *
    *         [A][A]   [B][B]
    *         [A][A]   [B][B]
    *
    *         Result: Do nothing.
    *
    *         [A][A]   [B][B]
    *         [A][A]   [B][B]
    *
    *
    *     2. Row AND column mismatch, even if partly contiguous
    *
    *         [A][A]
    *         [A][A][B][B]
    *               [B][B]
    *
    *         Result: Keep the two ranges separate.
    *
    *         [A][A]
    *         [A][A][B][B]
    *               [B][B]
    *
    *
    *     3. Ranges overlap partially
    *
    *         [A][A]
    *         [A](B)[B]
    *            [B][B]
    *
    *         Result: Break the existing range apart.
    *
    *         [A][A]
    *         [C][B][B]
    *            [B][B]
    *
    *
    *     4. Range does not go outside the bounds of the existing range
    *
    *         [A][A]                        [A][A][A]
    *         (B)(B)  or  [A](B)(B)[A]  or  [A](B)[A]
    *         (B)(B)      [A](B)(B)[A]      [A][A][A]
    *         [A][A]
    *
    *         Result: Subtract the new range from the existing one.
    *
    *         [A][A]                        [A][A][A]
    *                 or  [A]      [B]  or  [B]   [C]
    *                     [A]      [B]      [D][D][D]
    *         [B][B]
    *
    *******************************************************************************************************/

    for (size_t i = 0; i < ranges->size(); ++i)
    {
        for (size_t j = 0; j < ranges->size(); ++j)
        {
            if (IsRangeContiguous((*ranges)[i], (*ranges)[j]) && (IsRangeMatchingR((*ranges)[i], (*ranges)[j]) || IsRangeMatchingC((*ranges)[i], (*ranges)[j])))
            {
                (*ranges)[i].start.x = Min((*ranges)[i].start.x, (*ranges)[j].start.x);
                (*ranges)[i].start.y = Min((*ranges)[i].start.y, (*ranges)[j].start.y);
                (*ranges)[i].end.x = Max((*ranges)[i].end.x, (*ranges)[j].end.x);
                (*ranges)[i].end.y = Max((*ranges)[i].end.y, (*ranges)[j].end.y);
            }
        }
    }
}
bool IsCellInRange(Cell cell, Range range)
{
    return
        cell.x >= range.start.x &&
        cell.x <= range.end.x &&
        cell.y >= range.start.y &&
        cell.y <= range.end.y;
}
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
void OutlineRange(Range range, Color color, int thick = 1)
{
    int startX = PreviousColumnWidth(range.start.x);
    int startY = PreviousRowHeight(range.start.y);
    for (int i = 0; i < thick; ++i)
    {
        DrawRectangleLines(startX + i - 1, startY + i, g_ColumnWidths[range.end.x] - startX - i * 2 + 1, g_RowHeights[range.end.y] - startY - i * 2 + 1, color);
    }
}
void DrawHandle(Cell cell, Color color)
{
    DrawRectangle(g_ColumnWidths[cell.x] - 5, g_RowHeights[cell.y] - 4, 7, 7, WHITE);
    DrawRectangle(g_ColumnWidths[cell.x] - 4, g_RowHeights[cell.y] - 3, 6, 6, color);
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

    for (int i = 0; i < 26; ++i)
    {
        g_ColumnWidths.push_back(120 + 100 * i);
    }
    for (int i = 0; i < 100; ++i)
    {
        g_RowHeights.push_back(41 + 21 * i);
    }

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
    int drag_distance = 0;

    Cell startOfSelection;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        // Something must already be selected before selection can be eliminated
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (mouseMode == MouseMode::Dragging && region == Region::Edge)
            {
                // Column
                if (cell_x != -1)
                {
                    for (int i = cell_x; i < g_ColumnWidths.size(); ++i)
                        g_ColumnWidths[i] += drag_distance;
                }
                // Row
                else
                {
                    for (int i = cell_y; i < g_RowHeights.size(); ++i)
                        g_RowHeights[i] += drag_distance;
                }
            }
            mouseMode = MouseMode::None;
        }

        // Skip checking if we already know they're dragging something
        if (mouseMode != MouseMode::Dragging)
        {
            region = Region::Outside;

            // Find column
            if (GetMouseX() > 20)
            {
                for (int i = 0; i < g_ColumnWidths.size(); ++i)
                {
                    if (GetMouseX() <= g_ColumnWidths[i])
                    {
                        cell_x = i;
                        if (GetMouseY() <= 20 && (GetMouseX() + 8) >= g_ColumnWidths[i])
                            region = Region::Edge;
                        break;
                    }
                }
            }
            else
            {
                region = Region::Cell;
                cell_x = -1;
            }

            // Find row
            if (GetMouseY() > 20)
            {
                for (int i = 0; i < g_RowHeights.size(); ++i)
                {
                    if (GetMouseY() <= g_RowHeights[i])
                    {
                        cell_y = i;
                        if (GetMouseX() <= 20 && (GetMouseY() + 8) >= g_RowHeights[i])
                            region = Region::Edge;
                        break;
                    }
                }
            }
            else
            {
                if (region != Region::Edge)
                    region = Region::Cell;
                cell_y = -1;
            }

            if (cell_x != -1 && cell_y != -1)
                region = Region::Cell;

            if (region == Region::Edge)
            {
                if (cell_x != -1)
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                else
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
            }
            else if (!g_selection.empty())
            {
                Rectangle rec = { g_ColumnWidths[g_selection.back().end.x] - 5, g_RowHeights[g_selection.back().end.y] - 5, 7, 7 };
                if (CheckCollisionPointRec(GetMousePosition(), rec))
                    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
                else
                    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
            else
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                mouseMode = MouseMode::Dragging;

                if (region == Region::Cell)
                {
                    Cell cell = { cell_x, cell_y };

                    if (!g_selection.empty() && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) // Expand selection
                    {
                        if (cell_x == -1 && cell_y == -1)
                        {
                            g_selection.back() = RangeFromCellPair({ 0, startOfSelection.y }, { (int)g_ColumnWidths.size() - 1, 0});
                            mouseMode = MouseMode::Hovering;
                        }
                        if (cell_x == -1)
                        {

                        }
                        else if (cell_y == -1)
                        {

                        }
                        else
                            g_selection.back() = RangeFromCellPair(startOfSelection, cell);
                    }
                    else
                    {
                        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) // Add selection
                        {
                            startOfSelection = cell;
                            g_selection.push_back({ cell, cell });
                        }
                        else // Replace selection
                        {
                            startOfSelection = cell;
                            g_selection.clear();

                            if (cell_x == -1 || cell_y == -1)
                                mouseMode = MouseMode::Hovering;

                            if (cell_x == -1 && cell_y == -1)
                                g_selection.push_back({ { 0,0 }, { (int)g_ColumnWidths.size() - 1, (int)g_RowHeights.size() - 1 } });
                            else if (cell_x == -1)
                                g_selection.push_back({ { 0,cell_y }, { (int)g_ColumnWidths.size() - 1, cell_y } });
                            else if (cell_y == -1)
                                g_selection.push_back({ { cell_x,0 }, { cell_x, (int)g_RowHeights.size() - 1 } });
                            else
                                g_selection.push_back({ cell, cell });
                        }
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
            else if (region == Region::Cell)
            {
                int temp_x, temp_y;
                temp_x = temp_y = -1;

                // Find column
                if (GetMouseX() > 20)
                {
                    for (int i = 0; i < g_ColumnWidths.size(); ++i)
                    {
                        temp_x = i;
                        if (GetMouseX() <= g_ColumnWidths[i])
                            break;
                    }
                }

                // Find row
                if (GetMouseY() > 20)
                {
                    for (int i = 0; i < g_RowHeights.size(); ++i)
                    {
                        temp_y = i;
                        if (GetMouseY() <= g_RowHeights[i])
                            break;
                    }
                }

                if (cell_x != temp_x || cell_y != temp_y)
                {
                    cell_x = temp_x;
                    cell_y = temp_y;
                    g_selection.back() = RangeFromCellPair(startOfSelection, { cell_x, cell_y });
                }
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(WHITE);

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

            if (!(g_selection.size() == 1 && g_selection.back().start.x == g_selection.back().end.x && g_selection.back().start.y == g_selection.back().end.y))
            {
                for (size_t i = 0; i < g_selection.size(); ++i)
                {
                    HighlightRange(g_selection[i], SELECTION_BLUE);
                }
            }
            for (size_t i = 0; i < g_selection.size() - (mouseMode == MouseMode::Dragging ? 1 : 0); ++i)
            {
                OutlineRange(g_selection[i], CORNFLOWER_BLUE);
            }
            if (!g_selection.empty())
            {
                Cell startingCell = { (startOfSelection.x != -1 ? startOfSelection.x : 0), (startOfSelection.y != -1 ? startOfSelection.y : 0) };
                OutlineRange({ startingCell, startingCell }, CORNFLOWER_BLUE, 2);
                if (mouseMode != MouseMode::Dragging)
                DrawHandle(g_selection.back().end, CORNFLOWER_BLUE);
            }


            //if (!g_selection.empty())
            //    DrawText(TextFormat("$%c$%i:$%c$%i",'A' + g_selection.back().start.x, g_selection.back().start.y, 'A' + g_selection.back().end.x, g_selection.back().end.y),0,0,8,RED);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    // @TODO: Unload variables

    CloseWindow();

	return 0;
}