#include <string>
#include <vector>
#include <stack>
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

struct Context
{
    enum class Section
    {
        NA,
        Ribbon,
        Menu,
        FormulaBar,
        Frozen,
        WorkArea,
        // todo: add more
    } sect = Section::NA;

    union Subsection
    {
        enum class Subsection_NA { NA } na; // todo
        enum class Subsection_Ribbon { NA } rib; // todo
        enum class Subsection_Menu { NA } menu; // todo
        enum class Subsection_FormulaBar { NA, TextBox } fbar; // todo
        enum class Subsection_Frozen { NA, Cell, Edge } froz;
        enum class Subsection_WorkArea { NA, Cell, Edge, Handle } work;
    } sub;

    bool IsNA() { return sect == Section::NA; }
    bool IsRibbon() { return sect == Section::Ribbon; }
    bool IsMenu() { return sect == Section::Menu; }
    bool IsFormulaBar() { return sect == Section::FormulaBar; }
    bool IsFrozen() { return sect == Section::Frozen; }
    bool IsWorkArea() { return sect == Section::WorkArea; }


    bool IsFormula_TextBox() { return IsFormulaBar() && sub.fbar == Subsection::Subsection_FormulaBar::TextBox; }

    bool IsFrozen_Cell() { return IsFrozen() && sub.froz == Subsection::Subsection_Frozen::Cell; }
    bool IsWorkArea_Cell() { return IsWorkArea() && sub.work == Subsection::Subsection_WorkArea::Cell; }
    bool IsCell() { return IsFrozen_Cell() || IsWorkArea_Cell(); }

    bool IsFrozen_Edge() { return IsFrozen() && sub.froz == Subsection::Subsection_Frozen::Edge; }
    bool IsWorkArea_Edge() { return IsWorkArea() && sub.work == Subsection::Subsection_WorkArea::Edge; }
    bool IsEdge() { return IsFrozen_Edge() || IsWorkArea_Edge(); }

    bool IsWorkArea_Handle() { return IsWorkArea() && sub.work == Subsection::Subsection_WorkArea::Handle; }


    void Set_NA() { sect = Section::NA; sub.na = Subsection::Subsection_NA::NA; }

    void Set_Ribbon() { sect = Section::Ribbon; sub.rib = Subsection::Subsection_Ribbon::NA; }

    void Set_Menu() { sect = Section::Menu; sub.menu = Subsection::Subsection_Menu::NA; }

    void Set_FormulaBar() { sect = Section::FormulaBar; sub.fbar = Subsection::Subsection_FormulaBar::NA; }
    void Set_FormulaBar_TextBox() { sect = Section::FormulaBar; sub.fbar = Subsection::Subsection_FormulaBar::TextBox; }

    void Set_Frozen() { sect = Section::Frozen; sub.froz = Subsection::Subsection_Frozen::NA; }
    void Set_Frozen_Cell() { sect = Section::Frozen; sub.froz = Subsection::Subsection_Frozen::Cell; }
    void Set_Frozen_Edge() { sect = Section::Frozen; sub.froz = Subsection::Subsection_Frozen::Edge; }

    void Set_WorkArea() { sect = Section::WorkArea; sub.work = Subsection::Subsection_WorkArea::NA; }
    void Set_WorkArea_Cell() { sect = Section::WorkArea; sub.work = Subsection::Subsection_WorkArea::Cell; }
    void Set_WorkArea_Edge() { sect = Section::WorkArea; sub.work = Subsection::Subsection_WorkArea::Edge; }
    void Set_WorkArea_Handle() { sect = Section::WorkArea; sub.work = Subsection::Subsection_WorkArea::Handle; }

    // Returns success
    bool Set_Cell(Section fallback = Section::NA)
    {
        switch (sect)
        {
        case Section::Frozen: sub.froz = Subsection::Subsection_Frozen::Cell; return true;
        case Section::WorkArea: sub.work = Subsection::Subsection_WorkArea::Cell; return true;
        default:
            if (fallback == Section::Frozen || fallback == Section::WorkArea)
            {
                sect = fallback;
                return Set_Cell();
            }
            else
                return false;
        }
    }
    // Returns success
    bool Set_Edge(Section fallback = Section::NA)
    {
        switch (sect)
        {
        case Section::Frozen: sub.froz = Subsection::Subsection_Frozen::Edge; return true;
        case Section::WorkArea: sub.work = Subsection::Subsection_WorkArea::Edge; return true;
        default:
            if (fallback == Section::Frozen || fallback == Section::WorkArea)
            {
                sect = fallback;
                return Set_Edge();
            }
            else
                return false;
        }
    }
};

struct CellStyle
{
    int fontSize;
    Color fontColor = BLACK;

    // top, right, bottom, left
    bool border[4];
    bool background;
    Color borderColor[4]; // Gray if !border[i]
    Color backgroundColor; // White if !background
};
std::vector<CellStyle> g_Styles;

std::vector<int> g_ColumnWidths;
std::vector<int> g_RowHeights;

namespace UI
{
    namespace Ribbon
    {
        constexpr uint32_t g_height = 20;
    }
    namespace Menu
    {
        bool g_IsCollapsed = false;
        constexpr uint32_t g_height = 200;
        constexpr uint32_t g_height_Collapsed = 50;
        uint32_t MenuHeight()
        {
            if (g_IsCollapsed)
                return g_height_Collapsed;
            else
                return g_height;
        }
    }
    namespace FormulaBar
    {
        constexpr uint32_t g_height = 21;
    }
    namespace Frozen
    {
        constexpr uint32_t g_ColumnWidth = 50;
        constexpr uint32_t g_RowHeight = 20;
    }
}

struct CellAddress
{
    int x, y;
};
const char* AddressText_R(int row)
{
    if (row < 0)
        return "#REF!";

    return TextFormat("%i", 1 + row);
}
const char* AddressText_C(int column)
{
    if (column < 0)
        return "#REF!";

    if (column < 26)
        return TextFormat("%c", 'A' + column);
    else if (column < 26 * 26)
        return TextFormat("%c%c", 'A' + column / 26 - 1, 'A' + column % 26);
    else if (column < 26 * 26 * 26)
        return TextFormat("%c%c%c", 'A' + column / (26 * 26) - 1, 'A' + column / 26, 'A' + column % 26);
    else
        return "#VALUE!";
}
const char* AddressText(int row, int column)
{
    if (row < 0 || column < 0)
        return "#REF!";

    if (column < 26)
        return TextFormat("%c%i", 'A' + column, 1 + row);
    else if (column < 26 * 26)
        return TextFormat("%c%c%i", 'A' + column / 26 - 1, 'A' + column % 26, 1 + row);
    else if (column < 26 * 26 * 26)
        return TextFormat("%c%c%c%i", 'A' + column / (26 * 26) - 1, 'A' + column / 26, 'A' + column % 26, 1 + row);
}
struct Range
{
    CellAddress start, end;
};
std::vector<Range> g_Merged; // Entire merged range takes the style of the top-left cell

std::vector<std::vector<std::string>> g_cells;

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
Range RangeFromCellPair(CellAddress a, CellAddress b)
{
    return
        Range{
            CellAddress{
                Min(a.x, b.x),
                Min(a.y, b.y)
            },
            CellAddress{
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
bool IsCellInRange(CellAddress cell, Range range)
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
        return 50;
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
        DrawRectangle(x, y - 2, 50, 5, color);
        DrawLine(50, y, g_ColumnWidths.back(), y, color);
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
void DrawHandle(CellAddress cell, Color color)
{
    DrawRectangle(g_ColumnWidths[cell.x] - 5, g_RowHeights[cell.y] - 4, 7, 7, WHITE);
    DrawRectangle(g_ColumnWidths[cell.x] - 4, g_RowHeights[cell.y] - 3, 6, 6, color);
}

void DrawRibbon(size_t start_x, size_t start_y, size_t end_x)
{
    // todo
}
void DrawMenu(size_t start_x, size_t start_y, size_t end_x)
{

}
void DrawMenu_Collapsed(size_t start_x, size_t start_y, size_t end_x)
{

}
void DrawFrozen(size_t start_x, size_t start_y, size_t end_x, size_t end_y)
{
    size_t frozenColumnWidth = 50;
    size_t frozenRowHeight = 20;

    DrawRectangle(0, 0, frozenColumnWidth, g_RowHeights.back(), RAYWHITE);
    DrawRectangle(0, 0, g_ColumnWidths.back(), frozenRowHeight, RAYWHITE);
    DrawLine(frozenColumnWidth, 0, frozenColumnWidth, g_RowHeights.back(), DARKGRAY1);
    DrawLine(0, frozenRowHeight, g_ColumnWidths.back(), frozenRowHeight, DARKGRAY1);

    for (int i = 0; i < g_ColumnWidths.size(); ++i)
    {
        DrawLine(g_ColumnWidths[i], 0, g_ColumnWidths[i], frozenRowHeight, DARKGRAY1);
        DrawLine(g_ColumnWidths[i], 21, g_ColumnWidths[i], g_RowHeights.back(), LIGHTGRAY1);
        DrawText((i > 26 ? TextFormat("%c%c", 'A' + i / 26, 'A' + i % 26) : TextFormat("%c", 'A' + i)), (g_ColumnWidths[i] + PreviousColumnWidth(i)) / 2 - 3.5f, 5, 10, DARKGRAY3);
    }
    for (int i = 0; i < g_RowHeights.size(); ++i)
    {
        DrawLine(0, g_RowHeights[i], 50, g_RowHeights[i], DARKGRAY1);
        DrawLine(50, g_RowHeights[i], g_ColumnWidths.back(), g_RowHeights[i], LIGHTGRAY1);
        DrawText(TextFormat("%i", i), 25 - 3.5f, (g_RowHeights[i] + PreviousRowHeight(i)) / 2 - 2.5f, 10, DARKGRAY3);
    }
}
void DrawWorkArea(size_t start_x, size_t start_y, size_t end_x, size_t end_y)
{
    for (int i = 0; i < g_ColumnWidths.size(); ++i)
    {
        DrawLine(g_ColumnWidths[i], 0, g_ColumnWidths[i], 20, DARKGRAY1);
        DrawLine(g_ColumnWidths[i], 21, g_ColumnWidths[i], g_RowHeights.back(), LIGHTGRAY1);
        DrawText((i > 26 ? TextFormat("%c%c", 'A' + i / 26, 'A' + i % 26) : TextFormat("%c", 'A' + i)), (g_ColumnWidths[i] + PreviousColumnWidth(i)) / 2 - 3.5f, 5, 10, DARKGRAY3);
    }
    for (int i = 0; i < g_RowHeights.size(); ++i)
    {
        DrawLine(0, g_RowHeights[i], 50, g_RowHeights[i], DARKGRAY1);
        DrawLine(50, g_RowHeights[i], g_ColumnWidths.back(), g_RowHeights[i], LIGHTGRAY1);
        DrawText(TextFormat("%i", i), 25 - 3.5f, (g_RowHeights[i] + PreviousRowHeight(i)) / 2 - 2.5f, 10, DARKGRAY3);
    }
}

int main()
{
    int windowWidth = 1280;
    int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Spreadsheeter");
    SetTargetFPS(60);
    //ToggleFullscreen();

    /******************************************
    *   Load textures, shaders, and meshes    *
    ******************************************/

    for (int i = 0; i < 26; ++i)
    {
        g_ColumnWidths.push_back(150 + 100 * i);
    }
    for (int i = 0; i < 100; ++i)
    {
        g_RowHeights.push_back(41 + 21 * i);
    }

    Context context;
    context.sect = Context::Section::NA;
    context.sub.na = Context::Subsection::Subsection_NA::NA;

    enum class MouseMode
    {
        None,
        Hovering, // Used for highlighting resize anchors
        Dragging,
    } mouseMode = MouseMode::None;

    // -1 for none
    int cell_x = -1;
    int cell_y = -1;
    int drag_distance = 0;
    float timeOfLastClick = -1.0f;
    bool b_DoubleClicking;

    CellAddress startOfSelection;

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        b_DoubleClicking = false;

        // Release left click
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (mouseMode == MouseMode::Dragging)
            {
                if (context.IsFrozen_Edge())
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
                else if (context.IsCell() && g_selection.size() > 1)
                {
                    std::stack<size_t> redundant;
                    // Handle selection actions now that selection is confirmed
                    // Subtract/Combine selection
                    for (size_t i = 0; i < g_selection.size() - 1; ++i)
                    {
                        // Subtract
                        if (g_selection.back().start.x >= g_selection[i].start.x &&
                            g_selection.back().end.x <= g_selection[i].end.x &&
                            g_selection.back().start.y >= g_selection[i].start.y &&
                            g_selection.back().end.y <= g_selection[i].end.y)
                        {
                            // Identical
                            /************
                            * ( )( )( ) *
                            * ( )( )( ) *
                            * ( )( )( ) *
                            ************/
                            if (g_selection.back().start.x == g_selection[i].start.x &&
                                g_selection.back().start.y == g_selection[i].start.y &&
                                g_selection.back().end.x == g_selection[i].end.x &&
                                g_selection.back().end.y == g_selection[i].end.y)
                            {
                                redundant.push(i);
                            }

                            // Shave top
                            /************
                            * ( )( )( ) *
                            * [ ][ ][ ] *
                            * [ ][ ][ ] *
                            ************/
                            else if (g_selection.back().start.x == g_selection[i].start.x &&
                                g_selection.back().start.y == g_selection[i].start.y &&
                                g_selection.back().end.x == g_selection[i].end.x &&
                                g_selection.back().end.y <= g_selection[i].end.y)
                            {
                                g_selection[i].start.y = g_selection.back().end.y + 1;
                            }

                            // Shave bottom
                            /************
                            * [ ][ ][ ] *
                            * [ ][ ][ ] *
                            * ( )( )( ) *
                            ************/
                            else if (g_selection.back().start.x == g_selection[i].start.x &&
                                g_selection.back().start.y >= g_selection[i].start.y &&
                                g_selection.back().end.x == g_selection[i].end.x &&
                                g_selection.back().end.y == g_selection[i].end.y)
                            {
                                g_selection[i].end.y = g_selection.back().start.y - 1;
                            }

                            // Shave left
                            /************
                            * ( )[ ][ ] *
                            * ( )[ ][ ] *
                            * ( )[ ][ ] *
                            ************/
                            else if (g_selection.back().start.x == g_selection[i].start.x &&
                                g_selection.back().start.y == g_selection[i].start.y &&
                                g_selection.back().end.x <= g_selection[i].end.x &&
                                g_selection.back().end.y == g_selection[i].end.y)
                            {
                                g_selection[i].start.x = g_selection.back().end.x + 1;
                            }

                            // Shave right
                            /************
                            * [ ][ ]( ) *
                            * [ ][ ]( ) *
                            * [ ][ ]( ) *
                            ************/
                            else if (g_selection.back().start.x >= g_selection[i].start.x &&
                                g_selection.back().start.y == g_selection[i].start.y &&
                                g_selection.back().end.x == g_selection[i].end.x &&
                                g_selection.back().end.y == g_selection[i].end.y)
                            {
                                g_selection[i].end.x = g_selection.back().start.x - 1;
                            }

                            // Cut horizontal
                            /************
                            * [ ][ ][ ] *
                            * ( )( )( ) *
                            * [ ][ ][ ] *
                            ************/
                            else if (g_selection.back().start.x == g_selection[i].start.x &&
                                g_selection.back().start.y >= g_selection[i].start.y &&
                                g_selection.back().end.x == g_selection[i].end.x &&
                                g_selection.back().end.y <= g_selection[i].end.y)
                            {
                                g_selection.push_back(g_selection.back());
                                size_t j = g_selection.size() - 2;
                                g_selection[j] = g_selection[i];

                                g_selection[i].end.y = g_selection.back().start.y - 1;
                                g_selection[j].start.y = g_selection.back().end.y + 1;
                            }

                            // Cut vertical
                            /************
                            * [ ]( )[ ] *
                            * [ ]( )[ ] *
                            * [ ]( )[ ] *
                            ************/
                            else if (g_selection.back().start.x >= g_selection[i].start.x &&
                                g_selection.back().start.y == g_selection[i].start.y &&
                                g_selection.back().end.x <= g_selection[i].end.x &&
                                g_selection.back().end.y == g_selection[i].end.y)
                            {
                                g_selection.push_back(g_selection.back());
                                size_t j = g_selection.size() - 2;
                                g_selection[j] = g_selection[i];

                                g_selection[i].end.x = g_selection.back().start.x - 1;
                                g_selection[j].start.x = g_selection.back().end.x + 1;
                            }

                            // Hole punch
                            /************
                            * [ ][ ][ ] *
                            * [ ]( )[ ] *
                            * [ ][ ][ ] *
                            ************/
                            else if (g_selection.back().start.x >= g_selection[i].start.x &&
                                g_selection.back().start.y >= g_selection[i].start.y &&
                                g_selection.back().end.x <= g_selection[i].end.x &&
                                g_selection.back().end.y <= g_selection[i].end.y)
                            {
                                bool touches_left = g_selection.back().start.x == g_selection[i].start.x;
                                bool touches_top = g_selection.back().start.y == g_selection[i].start.y;
                                bool touches_right = g_selection.back().end.x == g_selection[i].end.x;
                                bool touches_bottom = g_selection.back().end.y == g_selection[i].end.y;

                                size_t partsAdded = (size_t)!touches_left + (size_t)!touches_right + (size_t)!touches_bottom - (size_t)touches_top;

                                if (partsAdded)
                                {
                                    Range temp = g_selection.back();
                                    g_selection.back() = g_selection[i];
                                    g_selection.reserve(g_selection.size() + partsAdded);
                                    for (size_t j = 0; j < partsAdded; ++j)
                                    {
                                        g_selection.push_back(g_selection[i]);
                                    }
                                    g_selection.back() = temp;
                                }

                                size_t j = (g_selection.size() - partsAdded) - 1;

                                /**************************
                                * left, top, right, bottom
                                *
                                * none 0000
                                * [i][i][i]
                                * [j]( )[k]
                                * [l][l][l]
                                *
                                * left 1000
                                * [i][i][i]
                                * ( )( )[j]
                                * [k][k][k]
                                * top 0100
                                * [i]( )[j]
                                * [i]( )[j]
                                * [k][k][k]
                                * right 0010
                                * [i][i][i]
                                * [j]( )( )
                                * [k][k][k]
                                * bottom 0001
                                * [i][i][i]
                                * [j]( )[k]
                                * [j]( )[k]
                                *
                                * left, top 1100
                                * ( )( )[i]
                                * ( )( )[i]
                                * [j][j][j]
                                * left, bottom 1001
                                * [i][i][i]
                                * ( )( )[j]
                                * ( )( )[j]
                                * top, right 0110
                                * [i]( )( )
                                * [i]( )( )
                                * [j][j][j]
                                * right, bottom 0101
                                * [i][i][i]
                                * [j]( )( )
                                * [j]( )( )
                                **************************/
                                // Prioratize keeping cells together horizontally
                                uint8_t branch = (touches_left << 3) | (touches_top << 2) | (touches_right << 1) | touches_bottom;
                                switch (branch)
                                {
                                case 0b0000: // none
                                    // Top
                                    g_selection[i].end.y = g_selection.back().start.y - 1;

                                    // Left
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].end.x = g_selection.back().start.x - 1;

                                    // Right
                                    g_selection[j + 1].start.y = g_selection.back().start.y;
                                    g_selection[j + 1].end.y = g_selection.back().end.y;
                                    g_selection[j + 1].start.x = g_selection.back().end.x + 1;

                                    // Bottom
                                    g_selection[j + 2].start.y = g_selection.back().end.y + 1;
                                    break;
                                case 0b1000: // left
                                    // Top
                                    g_selection[i].end.y = g_selection.back().start.y - 1;

                                    // Right
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].start.x = g_selection.back().end.x + 1;

                                    // Bottom
                                    g_selection[j + 1].start.y = g_selection.back().end.y + 1;
                                    break;
                                case 0b0100: // top
                                    // Left
                                    g_selection[i].start.y = g_selection.back().start.y;
                                    g_selection[i].end.y = g_selection.back().end.y;
                                    g_selection[i].end.x = g_selection.back().start.x - 1;

                                    // Right
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].start.x = g_selection.back().end.x + 1;

                                    // Bottom
                                    g_selection[j + 1].start.y = g_selection.back().end.y + 1;
                                    break;
                                case 0b0010: // right
                                    // Top
                                    g_selection[i].end.y = g_selection.back().start.y - 1;

                                    // Left
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].end.x = g_selection.back().start.x - 1;

                                    // Bottom
                                    g_selection[j + 1].start.y = g_selection.back().end.y + 1;
                                    break;
                                case 0b0001: // bottom
                                    // Top
                                    g_selection[i].end.y = g_selection.back().start.y - 1;

                                    // Left
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].end.x = g_selection.back().start.x - 1;

                                    // Right
                                    g_selection[j + 1].start.y = g_selection.back().start.y;
                                    g_selection[j + 1].end.y = g_selection.back().end.y;
                                    g_selection[j + 1].start.x = g_selection.back().end.x + 1;
                                    break;

                                case 0b1100: // left, top
                                    // Right
                                    g_selection[i].start.y = g_selection.back().start.y;
                                    g_selection[i].end.y = g_selection.back().end.y;
                                    g_selection[i].start.x = g_selection.back().end.x + 1;

                                    // Bottom
                                    g_selection[j].start.y = g_selection.back().end.y + 1;
                                    break;
                                case 0b1001: // left, bottom
                                    // Top
                                    g_selection[i].end.y = g_selection.back().start.y - 1;

                                    // Right
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].start.x = g_selection.back().end.x + 1;
                                    break;
                                case 0b0110: // top, right

                                    // Left
                                    g_selection[i].start.y = g_selection.back().start.y;
                                    g_selection[i].end.y = g_selection.back().end.y;
                                    g_selection[i].end.x = g_selection.back().start.x - 1;

                                    // Bottom
                                    g_selection[j].start.y = g_selection.back().end.y + 1;
                                    break;
                                case 0b0011: // right, bottom
                                    // Top
                                    g_selection[i].end.y = g_selection.back().start.y - 1;

                                    // Left
                                    g_selection[j].start.y = g_selection.back().start.y;
                                    g_selection[j].end.y = g_selection.back().end.y;
                                    g_selection[j].end.x = g_selection.back().start.x - 1;
                                    break;
                                }
                            }
                            startOfSelection = g_selection[i].start;
                            g_selection.pop_back();
                        }
                        // Combine
                        else if (g_selection.back().start.x == g_selection[i].start.x &&
                                 g_selection.back().end.x == g_selection[i].end.x)
                        {
                            // Prepend
                            if (g_selection.back().start.y == g_selection[i].end.y + 1)
                                g_selection.back().start.y = g_selection[i].start.y;

                            // Append
                            else if (g_selection.back().end.y + 1 == g_selection[i].start.y)
                                g_selection.back().end.y = g_selection[i].end.y;

                            // Do nothing
                            else
                                continue;

                            redundant.push(i);
                        }
                    }
                    
                    while (!redundant.empty())
                    {
                        g_selection.erase(g_selection.begin() + redundant.top());
                        redundant.pop();
                    }
                }
            }
            mouseMode = MouseMode::None;
        }

        // Current context is ignored while the mouse is held down
        if (mouseMode != MouseMode::Dragging)
        {
            // Determine context
            if (GetMouseY() <= UI::Ribbon::g_height)
            {
                context.Set_Ribbon();
                // Todo
            }
            else if (GetMouseY() <= UI::Ribbon::g_height + UI::Menu::MenuHeight())
            {
                context.Set_Menu();
                // Todo
            }
            else if ((GetMouseY() <= UI::Ribbon::g_height + UI::Menu::MenuHeight() + UI::Frozen::g_RowHeight) || (GetMouseX() <= UI::Frozen::g_ColumnWidth))
            {
                context.Set_Frozen();

                // Row
                if (GetMouseY() <= UI::Ribbon::g_height + UI::Menu::MenuHeight() + UI::Frozen::g_RowHeight)
                {
                    // Corner
                    if (GetMouseX() <= UI::Frozen::g_ColumnWidth)
                    {
                        // Todo
                    }
                    // Row
                    else
                    {
                        // Todo
                    }
                }
                // Column
                else
                {
                    // Todo
                }
            }
            else
            {
                context.Set_WorkArea();
                // Todo
            }

            // Find column
            if (GetMouseX() > UI::Frozen::g_ColumnWidth)
            {
                for (int i = 0; i < g_ColumnWidths.size(); ++i)
                {
                    if (GetMouseX() <= g_ColumnWidths[i])
                    {
                        cell_x = i;
                        if (GetMouseY() <= UI::Frozen::g_RowHeight && (GetMouseX() + 8) >= g_ColumnWidths[i])
                            context.Set_Edge(Context::Section::Frozen);
                        break;
                    }
                }
            }
            else
            {
                context.Set_Cell();
                cell_x = -1;
            }

            // Find row
            if (GetMouseY() > UI::Frozen::g_RowHeight)
            {
                for (int i = 0; i < g_RowHeights.size(); ++i)
                {
                    if (GetMouseY() <= g_RowHeights[i])
                    {
                        cell_y = i;
                        if (GetMouseX() <= UI::Frozen::g_ColumnWidth && (GetMouseY() + 8) >= g_RowHeights[i])
                            context.Set_Edge(Context::Section::Frozen);
                        break;
                    }
                }
            }
            else
            {
                if (!context.IsEdge())
                    context.Set_Cell();
                cell_y = -1;
            }

            if (cell_x != -1 && cell_y != -1)
                context.Set_Cell();

            if (context.IsEdge())
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

                if (GetTime() - timeOfLastClick <= 1.0f)
                    b_DoubleClicking = true;

                timeOfLastClick = GetTime();

                if (context.IsCell())
                {
                    CellAddress cell = { cell_x, cell_y };

                    if (!g_selection.empty() && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) // Expand selection
                    {
                        if (cell_x == -1 && cell_y == -1)
                        {
                            g_selection.back() = RangeFromCellPair({ 0, startOfSelection.y }, { (int)g_ColumnWidths.size() - 1, 0});
                            mouseMode = MouseMode::Hovering;
                        }
                        if (cell_x == -1)
                        {
                            // todo
                        }
                        else if (cell_y == -1)
                        {
                            // todo
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
                            // todo: Add support for frozen
                        }
                        else // Replace selection
                        {
                            startOfSelection = cell;
                            g_selection.clear();

                            if (cell_x == -1 || cell_y == -1)
                                mouseMode = MouseMode::Hovering;

                            if (cell_x == -1 && cell_y == -1)
                                g_selection.push_back({ { 0, 0 }, { (int)g_ColumnWidths.size() - 1, (int)g_RowHeights.size() - 1 } });
                            else if (cell_x == -1)
                                g_selection.push_back({ { 0, cell_y }, { (int)g_ColumnWidths.size() - 1, cell_y } });
                            else if (cell_y == -1)
                                g_selection.push_back({ { cell_x, 0 }, { cell_x, (int)g_RowHeights.size() - 1 } });
                            else
                                g_selection.push_back({ cell, cell });
                        }
                    }
                }
            }
        }

        // When mouse is held down
        if (mouseMode == MouseMode::Dragging)
        {
            if (context.IsEdge())
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
            else if (context.IsCell())
            {
                int temp_x, temp_y;
                temp_x = temp_y = -1;

                // Find column
                if (GetMouseX() > UI::Frozen::g_ColumnWidth)
                {
                    for (int i = 0; i < g_ColumnWidths.size(); ++i)
                    {
                        temp_x = i;
                        if (GetMouseX() <= g_ColumnWidths[i])
                            break;
                    }
                }

                // Find row
                if (GetMouseY() > UI::Frozen::g_RowHeight)
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
            
            // Draw frozen
            {
                DrawRectangle(0, 0, UI::Frozen::g_ColumnWidth, g_RowHeights.back(), RAYWHITE);
                DrawRectangle(0, 0, g_ColumnWidths.back(), UI::Frozen::g_RowHeight, RAYWHITE);
                DrawLine(UI::Frozen::g_ColumnWidth, 0, UI::Frozen::g_ColumnWidth, g_RowHeights.back(), DARKGRAY1);
                DrawLine(0, UI::Frozen::g_RowHeight, g_ColumnWidths.back(), UI::Frozen::g_RowHeight, DARKGRAY1);
            }

            // Draw cell divisions
            {
                for (int i = 0; i < g_ColumnWidths.size(); ++i)
                {
                    DrawLine(g_ColumnWidths[i], 0, g_ColumnWidths[i], 20, DARKGRAY1);
                    DrawLine(g_ColumnWidths[i], 21, g_ColumnWidths[i], g_RowHeights.back(), LIGHTGRAY1);
                    DrawText(AddressText_C(i), (g_ColumnWidths[i] + PreviousColumnWidth(i)) / 2 - 3.5f, 5, 10, DARKGRAY3);
                }
                for (int i = 0; i < g_RowHeights.size(); ++i)
                {
                    DrawLine(0, g_RowHeights[i], UI::Frozen::g_ColumnWidth, g_RowHeights[i], DARKGRAY1);
                    DrawLine(UI::Frozen::g_ColumnWidth, g_RowHeights[i], g_ColumnWidths.back(), g_RowHeights[i], LIGHTGRAY1);
                    DrawText(AddressText_R(i), 25 - 3.5f, (g_RowHeights[i] + PreviousRowHeight(i)) / 2 - 2.5f, 10, DARKGRAY3);
                }
            }

            if (context.IsEdge())
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
                        DrawRectangle(0, g_RowHeights[cell_y] - 4, UI::Frozen::g_ColumnWidth, 5, CORNFLOWER_BLUE);
                }
            }

            if (!(g_selection.size() == 1 && g_selection.back().start.x == g_selection.back().end.x && g_selection.back().start.y == g_selection.back().end.y))
            {
                for (size_t i = 0; i < g_selection.size(); ++i)
                {
                    HighlightRange(g_selection[i], SELECTION_BLUE);
                }
            }


            if (!g_selection.empty())
            {
                for (size_t i = 0; i < (mouseMode == MouseMode::Dragging ? g_selection.size() - 1 : g_selection.size()); ++i)
                {
                    OutlineRange(g_selection[i], CORNFLOWER_BLUE);
                }
                CellAddress startingCell = { (startOfSelection.x != -1 ? startOfSelection.x : 0), (startOfSelection.y != -1 ? startOfSelection.y : 0) };
                OutlineRange({ startingCell, startingCell }, CORNFLOWER_BLUE, 2);
                if ((mouseMode == MouseMode::Dragging ? cell_x == startOfSelection.x && cell_y == startOfSelection.y : true))
                    DrawHandle(g_selection.back().end, CORNFLOWER_BLUE);
            }


            //if (!g_selection.empty())
            //    DrawText(TextFormat("$%c$%i:$%c$%i",'A' + g_selection.back().start.x, g_selection.back().start.y, 'A' + g_selection.back().end.x, g_selection.back().end.y),0,0,8,RED);

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    CloseWindow();

	return 0;
}