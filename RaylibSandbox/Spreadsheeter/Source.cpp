#include <string>
#include <sstream>
#include <fstream>
#include <string_view>
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

bool IsHoldingShift()
{
    return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
}
bool IsHoldingCtrl()
{
    return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
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



struct Cell
{
    enum class Type
    {
        Blank,
        Text,
        Number,
        Formula,
    } type = Type::Blank;

    std::string str;
};

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
const char* AddressText(CellAddress cell)
{
    if (cell.y < 0 || cell.x < 0)
        return "#REF!";

    if (cell.x < 26)
        return TextFormat("%c%i", 'A' + cell.x, 1 + cell.y);
    else if (cell.x < 26 * 26)
        return TextFormat("%c%c%i", 'A' + cell.x / 26 - 1, 'A' + cell.x % 26, 1 + cell.y);
    else if (cell.x < 26 * 26 * 26)
        return TextFormat("%c%c%c%i", 'A' + cell.x / (26 * 26) - 1, 'A' + cell.x / 26, 'A' + cell.x % 26, 1 + cell.y);
}

struct RangeAddress
{
    CellAddress start, end;

    int Width()
    {
        return end.x - start.x;
    }
    int Height()
    {
        return end.y - start.y;
    }
    bool IsOneCell()
    {
        return start.x == end.x && start.y == end.y;
    }
};

// y, x
// Row, column
struct Range
{
    std::vector<std::vector<Cell>> map;

    Cell& At(int x, int y)
    {
        return map[y][x];
    }
    Cell& At(CellAddress address)
    {
        return map[address.y][address.x];
    }

    void Resize(size_t width, size_t height)
    {
        map.resize(height);
        for (size_t i = 0; i < height; ++i)
        {
            map[i].resize(width);
        }
    }

    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    // (   +   )
    void Push_Bottom()
    {
        map.push_back({});
        map.back().resize(Columns());
    }
    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    // (   +   )
    // (   +   )
    void Push_Bottom(size_t howMany)
    {
        size_t start = Rows();
        map.resize(Rows() + howMany);
        for (size_t i = start; i < Rows(); ++i)
        {
            map[i].resize(Columns());
        }
    }
    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    // (   -   )
    void Pop_Bottom()
    {
        map.pop_back();
    }
    // [ ][ ][ ]
    // [ ][ ][ ]
    // (   -   )
    // (   -   )
    void Pop_Bottom(size_t howMany)
    {
        map.resize(Rows() - howMany);
    }
    // [ ][ ][ ]
    // (   +   )
    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    void Insert_Row(size_t at)
    {
        std::vector<Cell> row;
        row.resize(Columns());
        map.insert(map.begin() + at, row);
    }
    // [ ][ ][ ]
    // (   +   )
    // (   +   )
    // [ ][ ][ ]
    // [ ][ ][ ]
    // [ ][ ][ ]
    void Insert_Row(size_t offset, size_t count)
    {
        map.reserve(Rows() + count);
        for (size_t i = 0; i < count; ++i)
        {
            Insert_Row(offset);
        }
    }
    // [ ][ ][ ]
    // (   -   )
    // [ ][ ][ ]
    // [ ][ ][ ]
    void Erase_Row(size_t at)
    {
        map.erase(map.begin() + at);
    }
    // [ ][ ][ ]
    // (   -   )
    // (   -   )
    // [ ][ ][ ]
    void Erase_Row(size_t offset, size_t count)
    {
        map.erase(map.begin() + offset, map.begin() + offset + count);
    }



    // [ ][ ][ ][ ](+)
    // [ ][ ][ ][ ](+)
    // [ ][ ][ ][ ](+)
    void Push_Right()
    {
        Cell cell_base;
        for (std::vector<Cell>& row : map)
        {
            row.push_back(cell_base);
        }
    }
    // [ ][ ][ ][ ](+)(+)
    // [ ][ ][ ][ ](+)(+)
    // [ ][ ][ ][ ](+)(+)
    void Push_Right(size_t howMany)
    {
        size_t newSize = Columns() + howMany;
        for (std::vector<Cell>& row : map)
        {
            row.resize(newSize);
        }
    }
    // [ ][ ][ ](-)
    // [ ][ ][ ](-)
    // [ ][ ][ ](-)
    void Pop_Right()
    {
        for (std::vector<Cell>& row : map)
        {
            row.pop_back();
        }
    }
    // [ ][ ](-)(-)
    // [ ][ ](-)(-)
    // [ ][ ](-)(-)
    void Pop_Right(size_t howMany)
    {
        size_t newSize = Columns() - howMany;
        for (std::vector<Cell>& row : map)
        {
            row.resize(newSize);
        }
    }
    // [ ](+)[ ][ ][ ]
    // [ ](+)[ ][ ][ ]
    // [ ](+)[ ][ ][ ]
    void Insert_Column(size_t at)
    {
        Cell cell_base;
        for (std::vector<Cell>& row : map)
        {
            row.insert(row.begin() + at, cell_base);
        }
    }
    // [ ](+)(+)[ ][ ][ ]
    // [ ](+)(+)[ ][ ][ ]
    // [ ](+)(+)[ ][ ][ ]
    void Insert_Column(size_t offset, size_t count)
    {
        size_t newSize = Columns() + count;
        for (std::vector<Cell>& row : map)
        {
            row.reserve(newSize);
        }
        for (size_t i = 0; i < count; ++i)
        {
            Insert_Column(offset);
        }
    }
    // [ ](-)[ ][ ]
    // [ ](-)[ ][ ]
    // [ ](-)[ ][ ]
    void Erase_Column(size_t at)
    {
        for (std::vector<Cell>& row : map)
        {
            row.erase(row.begin() + at);
        }
    }
    // [ ](-)(-)[ ]
    // [ ](-)(-)[ ]
    // [ ](-)(-)[ ]
    void Erase_Column(size_t offset, size_t count)
    {
        for (std::vector<Cell>& row : map)
        {
            row.erase(row.begin() + offset, row.begin() + offset + count);
        }
    }

    // Empty vector represents #REF! error
    Range SubRange(RangeAddress address)
    {
        Range output({});
        if (address.end.x >= Columns() ||
            address.end.y >= Rows() ||
            address.start.x < 0 ||
            address.start.y < 0)
        {
            return output;
        }

        output.map = map;

        if (address.start.y > 0)
            output.map.erase(output.map.begin(), output.map.begin() + address.start.y);

        if (address.start.x > 0)
        {
            for (int i = 0; i <= address.Height(); ++i)
            {
                output.map[i].erase(output.map[i].begin(), output.map[i].begin() + address.start.x);
            }
        }
        for (int i = 0; i <= address.Height(); ++i)
        {
            output.map[i].resize((size_t)address.Width() + 1);
        }

        output.map.resize((size_t)address.Height() + 1);

        return output;
    }

    size_t Rows()
    {
        return map.size();
    }
    size_t Columns()
    {
        return map[0].size();
    }
};

Range g_cells;


bool IsLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
int LetterValue(char c)
{
    if (c >= 'a')
        return c - ('a' - 1);
    else
        return c - ('A' - 1);
}
bool IsNumber(char c)
{
    return c >= '0' && c <= '9';
}
int NumberValue(char c)
{
    return c - '0';
}
#define ERR_RANGE { { -2,-2 }, { -2,-2 } }
RangeAddress ReferenceTextToAddress(const char* ref)
{
    RangeAddress address = RangeAddress{ { 0,0 }, { 0,0 } };

    size_t i = 0;
    bool locked = false;

    // Skip locks
    if (ref[i] == '$')
    {
        locked = true;
        ++i;
    }

    // A?:??
    while (IsLetter(ref[i]))
    {
        address.start.x = (address.start.x * 26) + LetterValue(ref[i]);
        ++i;
    }
    address.start.x--;
    if (address.start.x != -1)
        locked = false;

    // Skip locks
    if (ref[i] == '$')
    {
        // Locked without followup
        if (locked)
            return ERR_RANGE;

        locked = true;
        ++i;
    }

    // ?1:??
    while (IsNumber(ref[i]))
    {
        address.start.y = (address.start.y * 10) + NumberValue(ref[i]);
        ++i;
    }
    address.start.y--;
    if (address.start.y != -1)
        locked = false;

    // Address has second half?
    if (ref[i] == ':')
    {
        ++i;

        // Skip locks
        if (ref[i] == '$')
        {
            // Locked without followup
            if (locked)
                return ERR_RANGE;

            locked = true;
            ++i;
        }

        // ??:B?
        while (IsLetter(ref[i]))
        {
            address.end.x = (address.end.x * 26) + LetterValue(ref[i]);
            ++i;
        }
        address.end.x--;
        if (address.end.x != -1)
            locked = false;

        // Skip locks
        if (ref[i] == '$')
        {
            // Locked without followup
            if (locked)
                return ERR_RANGE;

            locked = true;
            ++i;
        }

        // ??:?2
        while (IsNumber(ref[i]))
        {
            address.end.y = (address.end.y * 10) + NumberValue(ref[i]);
            ++i;
        }
        address.end.y--;
        if (address.end.y != -1)
            locked = false;

        // Locked without followup
        if (locked)
            return ERR_RANGE;
    }
    else
    {
        address.end.x--;
        address.end.y--;
    }


    // A valid reference must have at least 2 coordinates:
    // - 1 X and 1 Y (both must be in the first half)
    // - 2 Xs (one must be in each half)
    // - 2 Ys (one must be in each half)
    // A -1 represents that the selected coordinate is missing.
    // Additionally, there should be no more characters in the reference by this point
    if (!ref[i])
    {
        // Put negatives at the end
        if (address.start.x == -1 && address.end.x != -1)
        {
            address.start.x = address.end.x;
            address.end.x = -1;
        }
        if (address.start.y == -1 && address.end.y != -1)
        {
            address.start.y = address.end.y;
            address.end.y = -1;
        }

        if ((address.start.x != -1 && address.end.x != -1) ||
            (address.start.y != -1 && address.end.y != -1))
        {
            return address;
        }
        else if (address.start.x != -1 && address.start.y != -1)
        {
            address.end = address.start;
            return address;
        }
    }

    return ERR_RANGE;
}
#undef ERR_RANGE


std::string ParamPack(std::string str)
{
    size_t start, end;
    bool searchingForStart = true;
    size_t layers = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '(')
        {
            ++layers;
            if (searchingForStart)
            {
                start = i;
                searchingForStart = false;
            }
        }
        if (str[i] == ')')
        {
            --layers;
            if (layers == 0)
            {
                end = i;
                break;
            }
        }
    }
    return str.substr(start + 1, end - start - 1);
}
std::string Param(std::string str, size_t param = 0)
{
    size_t start = 0;
    size_t end = str.length();
    size_t counter = 0;
    bool b_quote = false;
    bool confirmed = false;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '\"')
        {
            b_quote = !b_quote;
            continue;
        }
        if (str[i] == ',' && !b_quote)
        {
            if (counter == param)
            {
                end = i - 1;
                confirmed = true;
                break;
            }
            ++counter;
            if (counter == param)
            {
                start = i + 1;
            }
        }
    }
    if (confirmed)
        return str.substr(start, end - start + 1);
    else
        return "#ERR!";
}




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

struct SigNumber
{
    float number = 0.0f;
    int decimal_places = 0;
};
// Return NAN if unable to coerse a numeric value
SigNumber StringToNumber(const std::string_view str)
{
    float value = 0.0f;
    int frac = 0;
    for (char c : str)
    {
        if (c >= '0' && c <= '9')
        {
            float n = (float)(c - '0');
            if (frac)
            {
                value += (n / pow(10, frac));
                ++frac;
            }
            else
            {
                value = (value * 10.0f) + n;
            }
        }
        else if (c == '.')
        {
            if (frac)
                return { NAN, 0 };
            else
                frac = 1;
        }
        else if (c == ',' || c == '$')
        {
            continue;
        }
        else
            return { NAN, 0 };
    }
    return { value, frac };
}

void SetCell(Cell& cell, std::string_view in)
{
    if (in.empty())
    {
        cell.type = Cell::Type::Blank;
        cell.str = "";
    }
    else
    {
        if (in[0] == '=' || in[0] == '+')
            cell.type = Cell::Type::Formula;
        else
        {
            auto [n, d] = StringToNumber(in);

            if (isnan(n))
                cell.type = Cell::Type::Text;
            else
                cell.type = Cell::Type::Number;
        }
        cell.str = in;
    }
}

//void Save(const char* filename)
//{
//    std::ofstream file(filename);
//
//    for (const std::vector<Cell>& row : g_cells)
//    {
//        for (const Cell& cell : row)
//        {
//            switch (cell.type)
//            {
//            case Cell::Type::Text:
//            case Cell::Type::Formula:
//            {
//                std::string cell_text = cell.str;
//                size_t quotes = 0;
//                for (char c : cell_text)
//                {
//                    if (c == '\"')
//                        ++quotes;
//                }
//                if (quotes)
//                {
//                    cell_text.reserve(cell_text.size() + quotes);
//                    for (size_t i = cell_text.size(); i > 0; --i)
//                    {
//                        if (cell_text[i - 1] == '\"')
//                        {
//                            cell_text.insert(cell_text.begin() + i - 2, '\\');
//                        }
//                    }
//                }
//                file << "\"" << cell.str << "\"";
//            }
//            break;
//
//            case Cell::Type::Number:
//                file << cell.num; break;
//
//            case Cell::Type::Blank:
//                break;
//            }
//            file << ',';
//        }
//        file << ';';
//    }
//}
//void Load(const char* filename)
//{
//    std::ifstream file(filename);
//
//    for (std::vector<Cell>& row : g_cells)
//    {
//        for (Cell& cell : row)
//        {
//            std::string fileCell;
//            bool state = false;
//            while (!file.eof())
//            {
//                std::string buffer;
//                char next = file.peek();
//                if (next == '\"' && fileCell.back() != '\\')
//                {
//                    state = !state;
//                    if (!state)
//                    {
//                        break;
//                    }
//                }
//                else if (next == '\"' && fileCell.back() == '\\')
//                {
//                    fileCell.back() = '\"';
//                    file >> buffer;
//                }
//                file >> buffer;
//                fileCell.insert(fileCell.size(), buffer);
//            }
//
//            cell.Set(fileCell.c_str());
//        }
//    }
//}

namespace UI
{
    namespace Ribbon
    {
        constexpr uint32_t g_height = 20;

        uint32_t AbsoluteRibbonStart()
        {
            return 0;
        }
        uint32_t AbsoluteRibbonEnd()
        {
            return g_height;
        }
    }
    namespace Menu
    {
        bool g_IsCollapsed = false;
        constexpr uint32_t g_height = 100;
        constexpr uint32_t g_height_Collapsed = 50;

        uint32_t MenuHeight()
        {
            if (g_IsCollapsed)
                return g_height_Collapsed;
            else
                return g_height;
        }

        uint32_t AbsoluteMenuStart()
        {
            return Ribbon::AbsoluteRibbonEnd() + 1;
        }
        uint32_t AbsoluteMenuEnd()
        {
            return AbsoluteMenuStart() + MenuHeight();
        }
    }
    namespace FormulaBar
    {
        constexpr uint32_t g_height = 21;

        uint32_t AbsoluteFormulaBarStart()
        {
            return Menu::AbsoluteMenuEnd() + 1;
        }

        uint32_t AbsoluteFormulaBarEnd()
        {
            return AbsoluteFormulaBarStart() + g_height;
        }
    }
    namespace Frozen
    {
        constexpr uint32_t g_ColumnWidth = 50;
        constexpr uint32_t g_RowHeight = 20;

        uint32_t AbsoluteFrozenStart_Y()
        {
            return FormulaBar::AbsoluteFormulaBarEnd() + 1;
        }
        uint32_t AbsoluteFrozenEnd_Row()
        {
            return AbsoluteFrozenStart_Y() + g_RowHeight;
        }
        uint32_t AbsoluteFrozenEnd_Column()
        {
            return g_ColumnWidth;
        }
    }
    namespace WorkArea
    {
        std::vector<int> g_ColumnWidths;
        std::vector<int> g_RowHeights;

        // Start
        uint32_t AbsoluteWorkAreaStart_X()
        {
            return Frozen::AbsoluteFrozenEnd_Column() + 1;
        }
        uint32_t AbsoluteWorkAreaStart_Y()
        {
            return Frozen::AbsoluteFrozenEnd_Row() + 1;
        }

        // End
        uint32_t AbsoluteWorkAreaEnd_X()
        {
            return AbsoluteWorkAreaStart_X() + g_ColumnWidths.back();
        }
        uint32_t AbsoluteWorkAreaEnd_Y()
        {
            return AbsoluteWorkAreaStart_Y() + g_RowHeights.back();
        }

        // At
        uint32_t AbsoluteWorkAreaColumn_StartX(int column)
        {
            --column;
            if (column < 0)
                return AbsoluteWorkAreaStart_X() - 1;
            else if (column >= g_ColumnWidths.size())
                return AbsoluteWorkAreaEnd_X() - 1;
            else
                return AbsoluteWorkAreaStart_X() + g_ColumnWidths[column] - 1;
        }
        uint32_t AbsoluteWorkAreaColumn_EndX(int column)
        {
            if (column < 0)
                return AbsoluteWorkAreaStart_X();
            else if (column >= g_ColumnWidths.size())
                return AbsoluteWorkAreaEnd_X();
            else
                return AbsoluteWorkAreaStart_X() + g_ColumnWidths[column];
        }
        uint32_t AbsoluteWorkAreaRow_StartY(int row)
        {
            --row;
            if (row < 0)
                return AbsoluteWorkAreaStart_Y();
            else if (row >= g_RowHeights.size())
                return AbsoluteWorkAreaEnd_Y();
            else
                return AbsoluteWorkAreaStart_Y() + g_RowHeights[row];
        }
        uint32_t AbsoluteWorkAreaRow_EndY(int row)
        {
            if (row < 0)
                return AbsoluteWorkAreaStart_Y();
            else if (row >= g_RowHeights.size())
                return AbsoluteWorkAreaEnd_Y();
            else
                return AbsoluteWorkAreaStart_Y() + g_RowHeights[row];
        }

        // Width/height
        // Single cell
        uint32_t WorkAreaColumnWidth(int column)
        {
            return AbsoluteWorkAreaColumn_EndX(column) - AbsoluteWorkAreaColumn_StartX(column);
        }
        uint32_t WorkAreaRowHeight(int row)
        {
            return AbsoluteWorkAreaRow_EndY(row) - AbsoluteWorkAreaRow_StartY(row);
        }
        // Range
        uint32_t WorkAreaColumnRangeWidth(RangeAddress range)
        {
            return AbsoluteWorkAreaColumn_EndX(range.end.x) - AbsoluteWorkAreaColumn_StartX(range.start.x);
        }
        uint32_t WorkAreaRowRangeHeight(RangeAddress range)
        {
            return AbsoluteWorkAreaRow_EndY(range.end.y) - AbsoluteWorkAreaRow_StartY(range.start.y);
        }
        // Entire workspace
        uint32_t WorkAreaWidth()
        {
            return g_ColumnWidths.back();
        }
        uint32_t WorkAreaHeight()
        {
            return g_RowHeights.back();
        }
    }
}

std::vector<RangeAddress> g_Merged; // Entire merged range takes the style of the top-left cell

RangeAddress RangeFromCellPair(CellAddress a, CellAddress b)
{
    return
        RangeAddress{
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


void DrawVerticalLineRight(int column, Color color)
{
    DrawLine(
        UI::WorkArea::AbsoluteWorkAreaColumn_EndX(column),
        UI::WorkArea::AbsoluteWorkAreaStart_Y(),
        UI::WorkArea::AbsoluteWorkAreaColumn_EndX(column),
        UI::WorkArea::AbsoluteWorkAreaEnd_Y(),
        color);
}
void DrawHorizontalLineBottom(int row, Color color)
{
    DrawLine(
        UI::WorkArea::AbsoluteWorkAreaStart_X(),
        UI::WorkArea::AbsoluteWorkAreaRow_EndY(row),
        UI::WorkArea::AbsoluteWorkAreaEnd_X(),
        UI::WorkArea::AbsoluteWorkAreaRow_EndY(row),
        color);
}

// TODO: Add formatting support
void DrawCellText(const char* fmt, const char* text, CellAddress cell, int fontSize, Color color, bool editing = false)
{
    Rectangle cellRec =
    {
        UI::WorkArea::AbsoluteWorkAreaColumn_StartX(cell.x) + 3,
        UI::WorkArea::AbsoluteWorkAreaRow_StartY(cell.y) + (UI::WorkArea::WorkAreaRowHeight(cell.y) - fontSize),
        UI::WorkArea::WorkAreaColumnWidth(cell.x) - 3,
        UI::WorkArea::WorkAreaRowHeight(cell.y) - (UI::WorkArea::WorkAreaRowHeight(cell.y) - fontSize),
    };
    if (text[0] != '\0')
    {
        DrawTextRec(
            GetFontDefault(),
            text,
            cellRec,
            fontSize,
            fontSize / 10,
            true,
            color);
    }
}
// Put -1 in the axis that should be frozen
void DrawFrozenCellLabel(const char* text, CellAddress cell, int fontSize, Color color)
{
    // Frozen column (of row 123 labels)
    if (cell.x == -1)
        DrawText(text,
            3,
            UI::WorkArea::AbsoluteWorkAreaRow_StartY(cell.y) + 3,
            fontSize,
            color);

    // Frozen row (of column ABC labels)
    else if (cell.y == -1)
        DrawText(text,
            UI::WorkArea::AbsoluteWorkAreaColumn_StartX(cell.x) + 3,
            UI::Frozen::AbsoluteFrozenStart_Y() + 3,
            fontSize,
            color);
}
void DrawRangeFill(RangeAddress range, Color color)
{
    DrawRectangle(
        UI::WorkArea::AbsoluteWorkAreaColumn_StartX(range.start.x),
        UI::WorkArea::AbsoluteWorkAreaRow_StartY(range.start.y),
        UI::WorkArea::WorkAreaColumnRangeWidth(range),
        UI::WorkArea::WorkAreaRowRangeHeight(range),
        color);
}

// Listed clockwise
enum BorderSideFlags
{
    // Surrounding the range
    BORDER_TOP = 0b0001,
    BORDER_RIGHT = 0b0010,
    BORDER_BOTTOM = 0b0100,
    BORDER_LEFT = 0b1000,

    // Splitting internal cells
    BORDER_VERTICAL = 0b010000,
    BORDER_HORIZONTAL = 0b100000,
};
void DrawRangeBorder(RangeAddress range, BorderSideFlags sides, Color color)
{
    uint32_t startX = UI::WorkArea::AbsoluteWorkAreaColumn_StartX(range.start.x);
    uint32_t startY = UI::WorkArea::AbsoluteWorkAreaRow_StartY(range.start.y);
    uint32_t endX = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(range.end.x);
    uint32_t endY = UI::WorkArea::AbsoluteWorkAreaRow_EndY(range.end.y);

    if (sides & BORDER_TOP)
        DrawLine(startX, startY, endX, startY, color);

    if (sides & BORDER_LEFT)
        DrawLine(startX, startY, startX, endY, color);

    if (sides & BORDER_RIGHT)
        DrawLine(endX, startY, endX, endY, color);

    if (sides & BORDER_BOTTOM)
        DrawLine(startX, endY, endX, endY, color);

    // Horizontal line cycles vertically
    if (range.Height() > 1 && sides & BORDER_HORIZONTAL)
    {
        for (size_t i = range.start.y; i <= range.end.y; ++i)
        {
            int y = UI::WorkArea::AbsoluteWorkAreaRow_EndY(i);

            DrawLine(startX, y, endX, y, color);
        }
    }
    // Vertical line cycles horizontally
    if (range.Width() > 1 && sides & BORDER_VERTICAL)
    {
        for (size_t i = range.start.x; i <= range.end.x; ++i)
        {
            int x = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(i);

            DrawLine(x, startY, x, endY, color);
        }
    }
}
void DrawRangeOutline(RangeAddress range, int insetThickness, Color color)
{
    uint32_t startX = UI::WorkArea::AbsoluteWorkAreaColumn_StartX(range.start.x);
    uint32_t startY = UI::WorkArea::AbsoluteWorkAreaRow_StartY(range.start.y);
    uint32_t endX = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(range.end.x);
    uint32_t endY = UI::WorkArea::AbsoluteWorkAreaRow_EndY(range.end.y);
    uint32_t width = UI::WorkArea::WorkAreaColumnRangeWidth(range);
    uint32_t height = UI::WorkArea::WorkAreaRowRangeHeight(range);

    DrawRectangle(startX, startY, width, insetThickness, color);
    DrawRectangle(startX, startY, insetThickness, height, color);
    DrawRectangle(endX - insetThickness, startY, insetThickness, height, color);
    DrawRectangle(startX, endY - insetThickness, width, insetThickness, color);
}


bool IsCellInRange(CellAddress cell, RangeAddress range)
{
    return
        cell.x >= range.start.x &&
        cell.x <= range.end.x &&
        cell.y >= range.start.y &&
        cell.y <= range.end.y;
}
std::vector<RangeAddress> g_selection = {};

void DrawResizeBar(int x, int y, Color color)
{
    if (x > UI::Frozen::g_ColumnWidth) // Vertical
    {
        DrawRectangle(x - 3, UI::Frozen::AbsoluteFrozenStart_Y(), 5, UI::Frozen::g_RowHeight, color);
        DrawLine(x, UI::WorkArea::AbsoluteWorkAreaStart_Y(), x, UI::WorkArea::AbsoluteWorkAreaEnd_Y(), color);
    }
    else if (y > UI::Frozen::AbsoluteFrozenEnd_Row()) // Horizontal
    {
        DrawRectangle(0, y - 2, UI::Frozen::g_ColumnWidth, 5, color);
        DrawLine(UI::WorkArea::AbsoluteWorkAreaStart_X(), y, UI::WorkArea::AbsoluteWorkAreaEnd_X(), y, color);
    }
}
void DrawHandle(CellAddress cell, Color color)
{
    uint32_t centerPointX = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell.x);
    uint32_t centerPointY = UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell.y);
    DrawRectangle(centerPointX - 5, centerPointY - 4, 7, 7, WHITE);
    DrawRectangle(centerPointX - 4, centerPointY - 3, 6, 6, color);
}

void DrawRibbon(int windowWidth)
{
    DrawRectangle(0, 0, windowWidth, UI::Ribbon::g_height, RAYWHITE);
    DrawLine(0, UI::Ribbon::g_height, windowWidth, UI::Ribbon::g_height, DARKGRAY1);
}
void DrawMenu(int windowWidth)
{
    DrawRectangle(0, UI::Menu::AbsoluteMenuStart() + 1, windowWidth, UI::Menu::g_height - 1, RAYWHITE);
    DrawLine(0, UI::Menu::AbsoluteMenuStart() + 1 + UI::Menu::g_height, windowWidth, UI::Menu::AbsoluteMenuStart() + 1 + UI::Menu::g_height, DARKGRAY1);
}
void DrawMenu_Collapsed(int windowWidth)
{
    DrawRectangle(0, UI::Menu::AbsoluteMenuStart() + 1, windowWidth, UI::Menu::g_height - 1, RAYWHITE);
    DrawLine(0, UI::Menu::AbsoluteMenuStart() + UI::Menu::g_height_Collapsed, windowWidth, UI::Menu::AbsoluteMenuStart() + UI::Menu::g_height_Collapsed, DARKGRAY1);
}
void DrawFormulaBar(int windowWidth)
{
    DrawRectangle(0, UI::FormulaBar::AbsoluteFormulaBarStart() + 1, windowWidth, UI::FormulaBar::g_height - 1, WHITE);
    DrawLine(0, UI::FormulaBar::AbsoluteFormulaBarEnd() - 1, windowWidth, UI::FormulaBar::AbsoluteFormulaBarEnd() - 1, DARKGRAY1);
}
void DrawFrozen(std::vector<RangeAddress>* selection = nullptr)
{
    // The frozen row (row of columns)
    uint32_t startRowX = 0;
    uint32_t endRowX = UI::WorkArea::AbsoluteWorkAreaEnd_X();
    uint32_t startRowY = UI::Frozen::AbsoluteFrozenStart_Y();
    uint32_t endRowY = startRowY + UI::Frozen::g_RowHeight;
    uint32_t rowWidth = UI::WorkArea::WorkAreaWidth();
    uint32_t rowHeight = UI::Frozen::g_RowHeight;
    uint32_t rowHalfHeight = rowHeight / 2;

    // The frozen column (column of rows)
    uint32_t startColumnX = 0;
    uint32_t endColumnX = startColumnX + UI::Frozen::g_ColumnWidth;
    uint32_t startColumnY = UI::Frozen::AbsoluteFrozenStart_Y();
    uint32_t endColumnY = UI::WorkArea::AbsoluteWorkAreaEnd_Y();
    uint32_t columnWidth = UI::Frozen::g_ColumnWidth;
    uint32_t columnHeight = UI::WorkArea::WorkAreaHeight();
    uint32_t columnHalfWidth = columnWidth / 2;

    // Frozen cells fill
    DrawRectangle(startColumnX, startColumnY, columnWidth, columnHeight, LIGHTGRAY3); // Vertical
    DrawRectangle(startRowX, startRowY, rowWidth, rowHeight, LIGHTGRAY3); // Horizontal

    // Selected row/column
    if (selection)
    {
        for (RangeAddress range : *selection)
        {
            uint32_t highlightColumnX = UI::WorkArea::AbsoluteWorkAreaColumn_StartX(range.start.x);
            uint32_t highlightColumnWidth = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(range.end.x) - highlightColumnX;

            uint32_t highlightRowY = UI::WorkArea::AbsoluteWorkAreaRow_StartY(range.start.y);
            uint32_t highlightRowHeight = UI::WorkArea::AbsoluteWorkAreaRow_EndY(range.end.y) - highlightRowY;

            DrawRectangle(startRowX, highlightRowY, columnWidth, highlightRowHeight, LIGHTGRAY1); // Row
            DrawRectangle(highlightColumnX, startColumnY, highlightColumnWidth, rowHeight, LIGHTGRAY1); // Column
        }
    }

    // Frozen cells border lines
    DrawLine(endColumnX, startColumnY, endColumnX, endColumnY, DARKGRAY1); // Vertical
    DrawLine(startRowX, endRowY, endRowX, endRowY, DARKGRAY1); // Horizontal

    uint32_t frozenFontSize = 10;

    // Vertical (frozen row; cycling through columns) lines
    for (int i = 0; i < UI::WorkArea::g_ColumnWidths.size(); ++i)
    {
        uint32_t thisColumnX = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(i);
        DrawLine(thisColumnX, startRowY, thisColumnX, endRowY, DARKGRAY1);
        const char* columnWord = AddressText_C(i);
        uint32_t thisRowHalfWidth = UI::WorkArea::WorkAreaColumnWidth(i) / 2;
        //DrawText(columnWord, endColumnX + thisRowHalfWidth - 3.5f, startRowY + rowHalfHeight - 3.5f, frozenFontSize, DARKGRAY3);
        DrawFrozenCellLabel(columnWord, { i, -1 }, frozenFontSize, DARKGRAY3);
    }
    // Horizontal (frozen column; cycling through rows) lines
    for (int i = 0; i < UI::WorkArea::g_RowHeights.size(); ++i)
    {
        uint32_t thisRowY = UI::WorkArea::AbsoluteWorkAreaRow_EndY(i);
        DrawLine(startColumnX, thisRowY, endColumnX, thisRowY, DARKGRAY1);
        const char* rowNumber = AddressText_R(i);
        uint32_t thisColumnHalfHeight = UI::WorkArea::WorkAreaRowHeight(i) / 2;
        //DrawText(rowNumber, startColumnX + columnHalfWidth - 3.5f, endRowY + thisColumnHalfHeight - 2.5f, frozenFontSize, DARKGRAY3);
        DrawFrozenCellLabel(rowNumber, { -1, i }, frozenFontSize, DARKGRAY3);
    }
}
void DrawWorkArea(CellAddress exclusion = { -1, -1 })
{
    DrawRangeBorder(
        { { 0,0 }, { (int)UI::WorkArea::g_ColumnWidths.size(), (int)UI::WorkArea::g_RowHeights.size() } },
        BorderSideFlags(BORDER_HORIZONTAL | BORDER_VERTICAL | BORDER_RIGHT | BORDER_BOTTOM),
        LIGHTGRAY1);

    for (size_t y = 0; y < g_cells.Rows(); ++y)
    {
        for (size_t x = 0; x < g_cells.Columns(); ++x)
        {
            if (x == exclusion.x && y == exclusion.y)
                continue;

            if (g_cells.At(x, y).type == Cell::Type::Formula)
            {
                ReferenceTextToAddress(g_cells.At(x, y).str.c_str() + 1);
                DrawCellText("#.#", g_cells.At(x, y).str.c_str() + 1, { (int)x,(int)y }, 10, BLACK);
            }
            else
                DrawCellText("#.#", g_cells.At(x, y).str.c_str(), { (int)x,(int)y }, 10, BLACK);
        }
    }
}
void DrawSelection(std::vector<RangeAddress>* selection, CellAddress start)
{
    for (RangeAddress range : *selection)
    {
        DrawRangeFill(range, SELECTION_BLUE);
        DrawRangeOutline(range, 1, CORNFLOWER_BLUE);
    }
    DrawRangeOutline({ start, start }, 2, CORNFLOWER_BLUE);
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

    {
        g_cells.Resize(26, 100);
        for (int i = 0; i < g_cells.Columns(); ++i)
        {
            UI::WorkArea::g_ColumnWidths.push_back(100 * (i + 1));
        }
        for (int i = 0; i < g_cells.Rows(); ++i)
        {
            UI::WorkArea::g_RowHeights.push_back(21 * (i + 1));
        }
    }

    Context context;
    context.sect = Context::Section::NA;
    context.sub.na = Context::Subsection::Subsection_NA::NA;

    bool mouseDown = false;

    // -1 for none
    int cell_x = -1;
    int cell_y = -1;
    int drag_distance = 0;
    float timeOfLastClick = -1.0f;
    CellAddress placeOfLastClick = { -2, -2 };
    bool b_DoubleClicking;
    bool b_WaitingToEdit = false;
    bool b_EditingCell = false;

    CellAddress startOfSelection;

    std::string cellText = "";

    while (!WindowShouldClose())
    {
        /******************************************
        *   Simulate frame and update variables   *
        ******************************************/

        b_DoubleClicking = false;

        if (!b_WaitingToEdit)
            b_EditingCell = false;

        // Release left click
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (b_WaitingToEdit && placeOfLastClick.x == cell_x && placeOfLastClick.y == cell_y)
            {
                b_EditingCell = true;
                cellText = g_cells.At(startOfSelection).str;
            }

            if (mouseDown)
            {
                // Resize row/column
                if (context.IsFrozen_Edge())
                {
                    // Column
                    if (cell_x != -1)
                    {
                        for (int i = cell_x; i < UI::WorkArea::g_ColumnWidths.size(); ++i)
                            UI::WorkArea::g_ColumnWidths[i] += drag_distance;
                    }
                    // Row
                    else
                    {
                        for (int i = cell_y; i < UI::WorkArea::g_RowHeights.size(); ++i)
                            UI::WorkArea::g_RowHeights[i] += drag_distance;
                    }
                }
                // Update selection
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
                                    RangeAddress temp = g_selection.back();
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
                        else if (g_selection.back().start.y == g_selection[i].start.y &&
                            g_selection.back().end.y == g_selection[i].end.y)
                        {
                            // Prepend
                            if (g_selection.back().start.x == g_selection[i].end.x + 1)
                                g_selection.back().start.x = g_selection[i].start.x;

                            // Append
                            else if (g_selection.back().end.x + 1 == g_selection[i].start.x)
                                g_selection.back().end.x = g_selection[i].end.x;

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
            mouseDown = false;
        }

        // Update context
        // Realtime context is not updated while the mouse is held down
        // This is to prevent exiting the context while dragging causing the intent of the drag to be re/misinterpreted
        if (!mouseDown)
        {
            // Reset cursor
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);

            // Determine context

            // Ribbon
            if (GetMouseY() <= UI::Ribbon::AbsoluteRibbonEnd())
            {
                context.Set_Ribbon();
            }

            // Menu
            else if (GetMouseY() <= UI::Menu::AbsoluteMenuEnd())
            {
                context.Set_Menu();
            }

            // Formula Bar
            else if (GetMouseY() <= UI::FormulaBar::AbsoluteFormulaBarEnd())
            {
                context.Set_FormulaBar();
            }

            // Frozen
            else if (GetMouseY() <= UI::Frozen::AbsoluteFrozenEnd_Row() ||
                GetMouseX() <= UI::Frozen::g_ColumnWidth)
            {
                context.Set_Frozen_Cell();

                if (GetMouseX() <= UI::Frozen::g_ColumnWidth)
                {
                    cell_x = -1;
                }

                // Row
                if (GetMouseY() <= UI::Frozen::AbsoluteFrozenEnd_Row())
                {
                    cell_y = -1;

                    // Row
                    if (GetMouseX() > UI::Frozen::g_ColumnWidth)
                    {
                        for (cell_x = 0; cell_x < UI::WorkArea::g_ColumnWidths.size(); ++cell_x)
                        {
                            if (GetMouseX() <= UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell_x))
                            {
                                if ((GetMouseX() + 8) >= UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell_x))
                                {
                                    context.Set_Frozen_Edge();
                                    SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                                }
                                break;
                            }
                        }
                    }
                }

                // Column
                else // Will certainly be outside of the corner
                {
                    for (cell_y = 0; cell_y < UI::WorkArea::g_RowHeights.size(); ++cell_y)
                    {
                        if (GetMouseY() <= UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell_y))
                        {
                            if ((GetMouseY() + 8) >= UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell_y))
                            {
                                context.Set_Frozen_Edge();
                                SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                            }
                            break;
                        }
                    }
                }
            }

            // Workarea
            else
            {
                context.Set_WorkArea_Cell();
            }

            // Mouse press
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                b_EditingCell = false;
                mouseDown = true;
                bool b_mouseUnmoved = placeOfLastClick.x == cell_x && placeOfLastClick.y == cell_y;
                b_WaitingToEdit = b_mouseUnmoved;

                if (GetTime() - timeOfLastClick <= 1.0f && b_mouseUnmoved)
                {
                    b_DoubleClicking = true;
                    b_WaitingToEdit = context.IsWorkArea_Cell();
                }

                timeOfLastClick = GetTime();
                placeOfLastClick = { cell_x, cell_y };

                if (!IsHoldingShift())
                {
                    startOfSelection = { cell_x, cell_y };
                    CellAddress endOfSelection = startOfSelection;

                    int all_columns = UI::WorkArea::g_ColumnWidths.size() - 1;
                    int all_rows = UI::WorkArea::g_RowHeights.size() - 1;

                    if (cell_x == -1)
                    {
                        startOfSelection.x = 0;
                        endOfSelection.x = all_columns;
                    }
                    if (cell_y == -1)
                    {
                        startOfSelection.y = 0;
                        endOfSelection.y = all_rows;
                    }

                    if (!IsHoldingCtrl())
                        g_selection.clear();

                    g_selection.push_back({ startOfSelection, endOfSelection });
                }
            }
        }

        // Happens whether mouse is dragging or not
        switch (context.sect)
        {
        case Context::Section::Ribbon:
        {
            // todo
        }
        break;

        case Context::Section::Menu:
        {
            // todo
        }
        break;

        case Context::Section::FormulaBar:
        {
            // todo
        }
        break;

        case Context::Section::Frozen:
        {
            // Dragging to resize row/column
            if (mouseDown)
            {
                if (cell_x != -1) // Horizontal
                {
                    uint32_t end_x = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell_x);
                    uint32_t start_x = UI::WorkArea::AbsoluteWorkAreaColumn_StartX(cell_x);
                    drag_distance = GetMouseX() - end_x;
                    // Clamp
                    if (drag_distance < 0 && (end_x + drag_distance) < (start_x + 6))
                        drag_distance = start_x - end_x + 6;

                }
                else // Vertical
                {
                    uint32_t end_y = UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell_y);
                    uint32_t start_y = UI::WorkArea::AbsoluteWorkAreaRow_StartY(cell_y);
                    drag_distance = GetMouseY() - end_y;
                    // Clamp
                    if (drag_distance < 0 && (end_y + drag_distance) < (start_y + 6))
                        drag_distance = start_y - end_y + 6;

                }
            }
        }
        break;

        case Context::Section::WorkArea:
        {
            // Locate cell
            {
                for (cell_x = 0; cell_x < UI::WorkArea::g_ColumnWidths.size(); ++cell_x)
                {
                    if (UI::WorkArea::AbsoluteWorkAreaColumn_StartX(cell_x) <= GetMouseX() && GetMouseX() <= UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell_x))
                        break;
                }
                for (cell_y = 0; cell_y < UI::WorkArea::g_RowHeights.size(); ++cell_y)
                {
                    if (UI::WorkArea::AbsoluteWorkAreaRow_StartY(cell_y) <= GetMouseY() && GetMouseY() <= UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell_y))
                        break;
                }
            }

            // Whether mouse is on the handle
            if (!g_selection.empty())
            {
                // HACK: Range is being used for pixels instead of cells here
                RangeAddress handle;
                handle.start.x = UI::WorkArea::AbsoluteWorkAreaColumn_EndX(g_selection.back().end.x) - 5;
                handle.start.y = UI::WorkArea::AbsoluteWorkAreaRow_EndY(g_selection.back().end.y) - 5;
                handle.end.x = handle.start.x + 7;
                handle.end.y = handle.start.y + 7;

                if (GetMouseX() >= handle.start.x &&
                    GetMouseX() <= handle.end.x &&
                    GetMouseY() >= handle.start.y &&
                    GetMouseY() <= handle.end.y)
                {
                    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
                }
            }
        }
        break;

        default:
            break;
        }

        // Mouse has changed cells while held
        if (mouseDown && context.IsCell() && !g_selection.empty())
        {
            // No idea why but this seemed to not be getting short circuited when put in the same if statement together.
            // Mouse has changed cells
            if (g_selection.back().end.x != cell_x || g_selection.back().end.y != cell_y)
            {
                b_WaitingToEdit = b_EditingCell = false;

                CellAddress endOfSelection = { cell_x, cell_y };
                CellAddress proxyStart = startOfSelection;

                int all_columns = UI::WorkArea::g_ColumnWidths.size() - 1;
                int all_rows = UI::WorkArea::g_RowHeights.size() - 1;
                if (cell_x == -1)
                {
                    proxyStart.x = 0;
                    endOfSelection.x = all_columns;
                }
                if (cell_y == -1)
                {
                    proxyStart.y = 0;
                    endOfSelection.y = all_rows;
                }
                g_selection.back() = RangeFromCellPair(proxyStart, endOfSelection);
            }
        }

        // Arrow keys
        if (!b_EditingCell && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) && !g_selection.empty())
        {
            if (IsHoldingShift())
            {
                CellAddress temp;
                temp.x = (startOfSelection.x < g_selection.back().end.x ? g_selection.back().end.x : g_selection.back().start.x);
                temp.y = (startOfSelection.y < g_selection.back().end.y ? g_selection.back().end.y : g_selection.back().start.y);

                temp.x +=
                    (IsKeyPressed(KEY_RIGHT) && temp.x < UI::WorkArea::g_ColumnWidths.size() - 1)
                    - (IsKeyPressed(KEY_LEFT) && temp.x > 0);

                temp.y +=
                    (IsKeyPressed(KEY_DOWN) && temp.y < UI::WorkArea::g_RowHeights.size() - 1)
                    - (IsKeyPressed(KEY_UP) && temp.y > 0);

                g_selection.back() = RangeFromCellPair(startOfSelection, temp);
            }
            else
            {
                g_selection.clear();

                startOfSelection.x +=
                    (IsKeyPressed(KEY_RIGHT) && startOfSelection.x < UI::WorkArea::g_ColumnWidths.size() - 1)
                    - (IsKeyPressed(KEY_LEFT) && startOfSelection.x > 0);

                startOfSelection.y +=
                    (IsKeyPressed(KEY_DOWN) && startOfSelection.y < UI::WorkArea::g_RowHeights.size() - 1)
                    - (IsKeyPressed(KEY_UP) && startOfSelection.y > 0);

                g_selection.push_back({ startOfSelection, startOfSelection });
            }
        }

        if (b_EditingCell &&
            cell_x == g_selection.back().start.x &&
            cell_y == g_selection.back().start.y)
        {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
        }

        int c = GetCharPressed();
        bool ignoreEnter = false;
        if (!b_EditingCell)
        {
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE))
            {
                for (RangeAddress range : g_selection)
                {
                    for (int y = range.start.y; y <= range.end.y; ++y)
                    {
                        for (int x = range.start.x; x <= range.end.x; ++x)
                        {
                            SetCell(g_cells.At(x, y), "");
                        }
                    }
                }
            }
            else if (c >= ' ' && c <= '~' || IsKeyPressed(KEY_ENTER))
            {
                g_selection.clear();
                g_selection.push_back({ startOfSelection, startOfSelection });
                cellText = g_cells.At(startOfSelection).str;
                b_WaitingToEdit = true;
                b_EditingCell = true;
                ignoreEnter = true;
            }
        }
        if (b_EditingCell)
        {
            if (c >= ' ' && c <= '~')
                cellText.insert(cellText.size(), 1, c);

            if (IsKeyPressed(KEY_BACKSPACE) && !cellText.empty())
                cellText.pop_back();

            // Commit
            if (IsKeyPressed(KEY_ENTER) && !ignoreEnter)
            {
                b_EditingCell = false;
                SetCell(g_cells.At(startOfSelection), cellText);
                ++startOfSelection.y;
                ++g_selection.back().start.y;
                ++g_selection.back().end.y;
            }
        }

        /******************************************
        *   Draw the frame                        *
        ******************************************/

        BeginDrawing(); {

            ClearBackground(WHITE);

            if (b_EditingCell)
                DrawWorkArea(g_selection.back().start);
            else
                DrawWorkArea();

            if (!g_selection.empty())
            {
                if (b_EditingCell)
                {
                    DrawRangeOutline(g_selection.back(), 2, CORNFLOWER_BLUE);

                    if (!((int)((timeOfLastClick - GetTime()) * 2.0f) & 1))
                        DrawCellText("#.#", (cellText + '|').c_str(), g_selection.back().start, 10, BLACK, true);
                    else
                        DrawCellText("#.#", cellText.c_str(), g_selection.back().start, 10, BLACK, true);
                }
                else
                {
                    DrawSelection(&g_selection, startOfSelection);

                    if (!mouseDown)
                        DrawHandle(g_selection.back().end, CORNFLOWER_BLUE);
                }

                DrawFrozen(&g_selection);
            }
            else
                DrawFrozen();

            DrawFormulaBar(windowWidth);

            if (UI::Menu::g_IsCollapsed)
                DrawMenu_Collapsed(windowWidth);
            else
                DrawMenu(windowWidth);

            DrawRibbon(windowWidth);

            if (context.IsFrozen_Edge())
            {
                if (mouseDown)
                    DrawResizeBar(
                        (cell_x != -1 ? UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell_x) + drag_distance : 0),
                        (cell_y != -1 ? UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell_y) + drag_distance : 0),
                        CORNFLOWER_BLUE);
                else
                {
                    int barWidth = 5;
                    if (cell_y == -1)
                        DrawRectangle(UI::WorkArea::AbsoluteWorkAreaColumn_EndX(cell_x) - barWidth, UI::Frozen::AbsoluteFrozenStart_Y(), barWidth, UI::Frozen::g_RowHeight, CORNFLOWER_BLUE);
                    else
                        DrawRectangle(0, UI::WorkArea::AbsoluteWorkAreaRow_EndY(cell_y) - barWidth, UI::Frozen::g_ColumnWidth, barWidth, CORNFLOWER_BLUE);
                }
            }

            DrawText(TextFormat("Mouse coordinates: { %i, %i }", GetMouseX(), GetMouseY()), 0, 0, 8, RED);
            DrawText(TextFormat("Cell coordinates: { %i, %i }", cell_x, cell_y), 0, 10, 8, ORANGE);

            const char* contextText[] =
            {
                "NA",
                "Ribbon",
                "Menu",
                "FormulaBar",
                "Frozen",
                "WorkArea"
            };
            DrawText(contextText[(int)context.sect], 0, 20, 8, PURPLE);

            if (!g_selection.empty())
            {
                DrawText(TextFormat("Selection Range: %s%s:%s%s",
                    AddressText_C(g_selection.back().start.x), AddressText_R(g_selection.back().start.y),
                    AddressText_C(g_selection.back().end.x), AddressText_R(g_selection.back().start.y)
                ), 0, 30, 8, CORNFLOWER_BLUE);
            }

        } EndDrawing();
    }

    /******************************************
    *   Unload and free memory                *
    ******************************************/

    CloseWindow();

    return 0;
}