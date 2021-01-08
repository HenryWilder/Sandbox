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

constexpr char overlay[] =
R"TXT(+======================================================================================================================+
||   _    _      _ _         __          __        _     _ _       || debug data  /                                   ||
||  | |  | |    | | |        \ \        / /       | |   | | |      ||===========-`                                    ||
||  | |__| | ___| | | ___     \ \  /\  / /__  _ __| | __| | |      ||                                                 ||
||  |  __  |/ _ \ | |/ _ \     \ \/  \/ / _ \| '__| |/ _` | |      ||                                                 ||
||  | |  | |  __/ | | (_) |     \  /\  / (_) | |  | | (_| |_|      ||                                                 ||
||  |_|  |_|\___|_|_|\___( )     \/  \/ \___/|_|  |_|\__,_(_)      ||                                                 ||
||_______________--------|/-------------------------------------.  ||                                                 ||
|| communication \---------------------------------------------. `-||______________.._________________________________||
||===============-'                                             `--||_______________,---------------------------------||
||                                                                 || pretty stuff /                                  ||
||                                                                 ||============-`                                   ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
||                                                                 ||                                                 ||
+======================================================================================================================+)TXT";

void DrawOverlay()
{
    printf("\x1b[1;1H\x1b[40;33m%");
    for (size_t i = 0; i < sizeof(overlay); ++i)
    {
        if (overlay[i] != ' ') printf("%c", overlay[i]);
        else printf("\x1b[1C");
    }
}

char* debugLines[4];
void PushUpDebug(const std::string& line)
{
    debugLines[4];
}

void SayDebug(const std::string& str, const Color8Bit color)
{
    constexpr int DebugPos[] = { 71, 4 };
    constexpr int maxLines = 4;
    constexpr int wrapWidth = sizeof(">                                              ");
    char* linePart = "";
    const char format[] = "\x1b[4;71H%s%s\x1b[40;33m";
    sprintf_s(linePart, sizeof(format), format, ColorEsc(color, false, false).c_str(), str.c_str());
    printf(linePart);
}
void CompleteDebugLine()
{

}

void ClearComm()
{
    DrawOverlay();
}
void SayComm(const std::string& str, const Color8Bit color)
{
    constexpr int CommsPos[] = { 3, 11 };
    constexpr int maxLines = 18;
    constexpr int wrapWidth = sizeof(">                                                              ");
    printf("\x1b[11;4H%s%s\x1b[40;33m", ColorEsc(color, false, false).c_str(), str.c_str());
}
void SayPretty(const std::string& str, const Color8Bit color)
{
    constexpr int prettyStuffPos[] = { 71, 13 };
    constexpr int wrapWidth = sizeof(">                                              ");
    printf("\x1b[13;71H%s%s\x1b[40;33m", ColorEsc(color, false, false).c_str(), str.c_str());
}
