#if 0
#include "GUI.h"

char DensityRamp(const double value)
{
    constexpr const char ramp[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\" ^ `'. ";
    int pos = (int)(value * double(sizeof(ramp) - 1));
    return ramp[pos];
}

char DensityRamp(const float value)
{
    return DensityRamp((double)value);
}

Color8Bit operator+(Color8Bit l, Color8Bit r)
{
    return (Color8Bit)((int)l + (int)r);
}

std::string ColorEsc(const Color8Bit color, const bool bright, const bool background)
{
    const char* format = "\x1B[%im";
    char code[sizeof(format) - 1] = "";
    int colorVal = (int)color;
    if (bright) colorVal += (int)Color8Bit::Bright_Offset;
    if (background) colorVal += (int)Color8Bit::BG_Offset;
    sprintf_s(code, format, colorVal);
    printf(code);
    return code;
}

void InsertEscColor(std::string& buffer, const Color8Bit color, const size_t position)
{

}
void SetPrintColor8Bit(Color8Bit color)
{
    printf("\x1b[%im", (int)color);
}
void ResetPrintColor()
{
    SetPrintColor8Bit(Color8Bit::Yellow);
}
void SetPrintColorRGB(unsigned char r, unsigned char g, unsigned char b)
{
    printf("\x1b[48;2;%i;%i;%im", r, g, b);
}
void SetPrintPos(int x, int y)
{
    printf("\x1b[%i;%iH", y + 1, x + 1);
}



constexpr size_t g_debugLineCount = 5;
constexpr size_t g_commLineCount = 19;
constexpr size_t g_PrettyLineCount = 19;

LineData FindPositionInOverlay(PosSymbol symbol, int index)
{
    char search[] = "x00";
    search[0] = (char)symbol;
    if (index > 9) ++search[1];
    search[2] += index % 10;
    size_t pos = g_overlay.find(search);
    size_t width = g_overlay.find('<', pos) - pos;
    return LineData{ pos, width + 1 };
}

void PrepareLine(LineData line)
{
    SetPrintPos(line.x(), line.y());
    for (int i = 0; i < line.width; ++i)
    {
        printf(" ");
    }
    SetPrintPos(line.x(), line.y());
}

void DrawOverlay()
{
    SetPrintPos(0,0);
    ResetPrintColor();
    printf(g_overlay.c_str());
}

DebugLine g_debugLines[g_debugLineCount];

void SayDebug(DebugColor color, const char* fmt...)
{
    {
        std::ofstream log("log.txt", std::ofstream::out | std::ofstream::app);
        switch (color)
        {
        case DebugColor::Msg:
            log << "Info: ";
            break;
        case DebugColor::Warning:
            log << "Warning: ";
            break;
        case DebugColor::Critical:
            log << "Critical: ";
            break;
        }
        log << str << std::endl;
        log.close();

        static int s_msgNumber = 0;
        char numberInsert[] = "[00] ";
        sprintf_s(numberInsert, "[%#02i] ", ++s_msgNumber);
        std::string _str = numberInsert + str;
        for (int i = 4; i > 0; --i)
        {
            g_debugLines[i] = g_debugLines[i - 1];
        }
        g_debugLines[0] = { _str, color };
    }

    {
        for (int line = 4; line >= 0; --line)
        {
            const auto& debug = g_debugLines[line];

            LineData data = FindPositionInOverlay(PosSymbol::Debug, line);
            PrepareLine(data);

            if (!debug.m_str.empty())
            {
                switch (debug.m_color)
                {
                default:
                case DebugColor::Msg:
                    SetPrintColor8Bit(Color8Bit::Yellow);
                    break;
                case DebugColor::Warning:
                    SetPrintColor8Bit(Color8Bit::Bright_Offset + Color8Bit::Yellow);
                    break;
                case DebugColor::Critical:
                    SetPrintColor8Bit(Color8Bit::Red);
                    break;
                }

                if (debug.m_str.length() > data.width)
                    printf((debug.m_str.substr(0, data.width - 3) + "...").c_str());
                else
                    printf(debug.m_str.c_str());
            }
        }
        ResetPrintColor();
    }
}

std::string g_commLines[g_commLineCount];

void ClearComm()
{
    for (std::string& line : g_commLines)
    {
        line.clear();
    }
}
void PushComm(const char* str)
{
    constexpr int maxComm = g_commLineCount - 1;
    if (!str.empty())
    {
        size_t start = str.find_first_not_of(" \t\n");
        if (start != str.npos) str = str.substr(start);
        else return;

        int line = 0;
        if (!g_commLines[maxComm].empty())
        {
            // New line
            for (int i = 0; i < maxComm; ++i)
            {
                g_commLines[i] = g_commLines[i + 1];
            }
            line = maxComm;
        }
        else
        {
            // Fill in
            for (int i = 0; i <= maxComm; ++i)
            {
                if (g_commLines[i].empty())
                {
                    line = i;
                    break;
                }
            }
        }

        auto[pos, space] = FindPositionInOverlay(PosSymbol::Comm, line);

        if (str.length() > space)
        {
            std::string thisLine = str.substr(0ull, space);
            size_t _space = thisLine.rfind(" ");
            if (_space < space) space = _space;
            thisLine = thisLine.substr(0ull, space);
            g_commLines[line] = thisLine;
            PushComm(str.substr(space));
        }
        else
        {
            g_commLines[line] = str;
        }
    }
}

void SayComm(const char* fmt...)
{
    va_list args;

    PushComm(str);

    delete args;

    {
        ResetPrintColor(); // Color guard
        for (int line = 0; line < g_commLineCount; ++line)
        {
            PrepareLine(FindPositionInOverlay(PosSymbol::Comm, line));
            const std::string& str = g_commLines[line];
            if (!str.empty()) printf(str.c_str());
        }
    }
}

void SayPretty(const Color8Bit color, const char* str...)
{
    std::istringstream stream(str);
    std::string line;
    int i = 0;
    LineData reference = FindPositionInOverlay(PosSymbol::Pretty, 2);
    while (std::getline(stream, line, '\n') && i < g_PrettyLineCount)
    {
        LineData data = FindPositionInOverlay(PosSymbol::Pretty, i++);
        PrepareLine(data);
        printf(line.substr((size_t)data.x() - (size_t)reference.x(), data.width).c_str());
    }
    for (; i < g_PrettyLineCount; ++i)
    {
        PrepareLine(FindPositionInOverlay(PosSymbol::Pretty, i));
    }
}

int LineData::x()
{
    return (int)(pos % 121ull);
}
int LineData::y()
{
    return (int)(pos / 121ull);
}
#endif