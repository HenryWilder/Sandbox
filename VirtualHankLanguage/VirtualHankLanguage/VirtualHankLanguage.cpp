#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <ctime>
#include <iostream>

#pragma region Windows functions

#include <windows.h>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>

void Open(std::string& str)
{

    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    LPWSTR commandline = new WCHAR[MAX_PATH];
    for (size_t i = 0; i < str.size(); ++i)
    {
        commandline[i] = (WCHAR)(str[i]);
    }
    for (size_t i = str.size(); i < MAX_PATH; ++i)
    {
        commandline[i] = NULL;
    }
    int attempt = 0;
RETRY:
    BOOL success = CreateProcess(
        NULL,
        commandline,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &startupInfo,
        &processInfo
    );
    if (success)
    {
        //WaitForSingleObject(processInfo.hProcess, INFINITE);

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }
    else
    {
        int response = MessageBox(GetConsoleWindow(), L"The process could not be started...", L"Oops", 5L | 30L);
        if (response == IDRETRY)
        {
            if (attempt++ < 5) goto RETRY;
        }
    }
    delete[] commandline;
}

POINT GetMousePos()
{
    POINT p;
    if (GetCursorPos(&p)) return p;
    else return { 0,0 };
}

void SimulateMouseMove(long x, long y)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0; // Pleaseeeee don't mess with this... it makes the monitor go funky...
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(input));
}

void SimulateMouseGoto(long x, long y)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0; // Pleaseeeee don't mess with this... it makes the monitor go funky...
    input.mi.dx = x * 34;
    input.mi.dy = y * 61;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
}

void SimulateMouseClick()
{
    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));
    Sleep(10);
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

#pragma endregion


const char* manualMode = 1 + R"(
STEPS FOR SYNCING SHAREPOINT
============================
0. In the bottom-left corner of your Internet Explorer window, you should see a button/link that says
   "Return to classic SharePoint"
   Click it.
1. Once the page finishes loading, click on the "LIBRARY" tab/button in the top-left of the window.
   It will be in the light-gray bar, and to the right of the "FILES" tab/button.
2. This will unroll an actions panel. On that panel, there will be a "Sync" button. Click it.
3. A message box will appear in the center of your screen. Press "Allow".
   The message box is simply making sure you want to sync SharePoint.
If you've completed all these steps, SharePoint should be syncing!)";

constexpr POINT center = { 960, 540 };
constexpr POINT returnToClassic = { 90, 1013 };
constexpr POINT library = { 150, 150 };
constexpr POINT sync = { 600, 200 };
constexpr POINT allow = { 1000, 570 };

const char* g_FileName_Log = "log.txt"; // Name of the output log file
const char* g_FileName_Calibrate = "calibrate.vhid"; // Name of the calibration instruction document
const char* g_Filename_StoredData = "userdata.txt"; // Name of the file where user data is stored

#pragma region Graphics

// Credit: https://blog.kazitor.com/2014/12/portal-ascii/
const char* g_AsciiGraphics[]
{
    1 + R"(
             .,-:;//;:=,
         . :H@@@MM@M#H/.,+%;,
      ,/X+ +M@@M@MM%=,-%HMMM@X/,
     -+@MM; $M@@MH+-,;XMMMM@MMMM@+-
    ;@M@@M- XM@X;. -+XXXXXHHH@M@M#@/.
  ,%MM@@MH ,@%=            .---=-=:=,.
  -@#@@@MX .,              -%HX$$%%%+;
 =-./@M@M$                  .;@MMMM@MM:
 X@/ -$MM/                    .+MM@@@M$
,@M@H: :@:                    . -X#@@@@-
,@@@MMX, .                    /H- ;@M@M=
.H@@@@M@+,                    %MM+..%#$.
 /MMMM@MMH/.                  XM@MH; -;
  /%+%$XHH@$=              , .H@@@@MX,
   .=--------.           -%H.,@@@@@MX,
   .%MM@@@HHHXX$$$%+- .:$MMX -M@@MM%.
     =XMMM@MM@MM#H;,-+HMM@M+ /MMMX=
       =%@M@M#@$-.=$@MM@@@M; %M%=
         ,:+$+-,/H#MMMMMMM@- -,
               =++%%%%+/:-.
)",

    1 + R"(
             =+$HM####@H%;,
          /H###############M$,
          ,@################+
           .H##############+
             X############/
              $##########/
               %########/
                /X/;;+X/

                 -XHHX-
                ,######,
#############X  .M####M.  X#############
##############-   -//-   -##############
X##############%,      ,+##############X
-##############X        X##############-
 %############%          %############%
  %##########;            ;##########%
   ;#######M=              =M#######;
    .+M###@,                ,@###M+.
       :XH.                  .HX:
)",

1 + R"(
                 =/;;/-
                +:    //
               /;      /;
              -X        H.
.//;;;:;;-,   X=        :+   .-;:=;:;%;.
M-       ,=;;;#:,      ,:#;;:=,       ,@
:%           :%.=/++++/=.$=           %=
 ,%;         %/:+/;,,/++:+/         ;+.
   ,+/.    ,;@+,        ,%H;,    ,/+,
      ;+;;/= @.  .H##X   -X :///+;
      ;+=;;;.@,  .XM@$.  =X.//;=%/.
   ,;:      :@%=        =$H:     .+%-
 ,%=         %;-///==///-//         =%,
;+           :%-;;;;;;;;-X-           +:
@-      .-;;;;M-        =M/;;;-.      -X
 :;;::;;-.    %-        :+    ,-;;-;:==
              ,X        H.
               ;/      %=
                //    +;
                 ,////,
)",

1 + R"(
                          .,---.
                        ,/XM#MMMX;,
                      -%##########M%,
                     -@######%  $###@=
      .,--,         -H#######$   $###M:
   ,;$M###MMX;     .;##########$;HM###X=
,/@###########H=      ;################+
-+#############M/,      %##############+
%M###############=      /##############:
H################      .M#############;.
@###############M      ,@###########M:.
X################,      -$=X#######@:
/@##################%-     +######$-
.;##################X     .X#####+,
 .;H################/     -X####+.
   ,;X##############,       .MM/
      ,:+$H@M#######M#$-    .$$=
           .,-=;+$@###X:    ;/=.
                  .,/X$;   .::,
                      .,    ..
)",

1 + R"(
                     -$-
                    .H##H,
                   +######+
                .+#########H.
              -$############@.
            =H###############@  -X:
          .$##################:  @#@-
     ,;  .M###################;  H###;
   ;@#:  @###################@  ,#####:
 -M###.  M#################@.  ;######H
 M####-  +###############$   =@#######X
 H####$   -M###########+   :#########M,
  /####X-   =########%   :M########@/.
    ,;%H@X;   .$###X   :##MM@%+;:-
                 ..
  -/;:-,.              ,,-==+M########H
 -##################@HX%%+%%$%%%+:,,
    .-/H%%%+%%$H@###############M@+=:/+:
/XHX%:#####MH%=    ,---:;;;;/&&XHM,:###$
$@#MX %+;-                           .
)",

1 + R"(
                                     :X-
                                  :X###
                                ;@####@
                              ;M######X
                            -@########$
                          .$##########@
                         =M############-
                        +##############$
                      .H############$=.
         ,/:         ,M##########M;.
      -+@###;       =##########M;
   =%M#######;     :#########M/
-$M###########;   :########/
 ,;X###########; =#######$.
     ;H#########+######M=
       ,+#############+
          /M########@-
            ;M#####%
              +####:
               ,$M-
)",

    1 + R"(
            .+
             /M;
              H#@:              ;,
              -###H-          -@/
               %####$.  -;  .%#X
                M#####+;#H :M#M.
..          .+/;%#############-
 -/%H%+;-,    +##############/
    .:$M###MH$%+############X  ,--=;-
        -/H#####################H+=.
           .+#################X.
         =%M####################H;.
            /@###############+;;/%%;,
         -%###################$
       ;H######################M=
    ,%#####MH$%;+#####M###-/@####%
  :$H%+;=-      -####X.,H#   -+M##@-
 .              ,###;    ;      =$##+
                .#H,               :XH,
                 +                   .;-
)",

    1 + R"(
           .-;+$XHHHHHHX$+;-.
        ,;X@@X%/;=----=:/%X@@X/,
      =$@@%=.              .=+H@X:
    -XMX:                      =XMX=
   /@@:                          =H@+
  %@X,                            .$@$
 +@X.                               $@%
-@@,                                .@@=
%@%                                  +@$
H@:                                  :@H
H@:         :HHHHHHHHHHHHHHHHHHX,    =@H
%@%         ;@M@@@@@@@@@@@@@@@@@H-   +@$
=@@,        :@@@@@@@@@@@@@@@@@@@@@= .@@:
 +@X        :@@@@@@@@@@@@@@@M@@@@@@:%@%
  $@$,      ;@@@@@@@@@@@@@@@@@M@@@@@@$.
   +@@HHHHHHH@@@@@@@@@@@@@@@@@@@@@@@+
    =X@@@@@@@@@@@@@@@@@@@@@@@@@@@@X=
      :$@@@@@@@@@@@@@@@@@@@M@@@@$:
        ,;$@@@@@@@@@@@@@@@@@@X/-
           .-;+$XXHHHHHX$+;-.
)",

    1 + R"(
            ,:/+/-
            /M/              .,-=;//;-
       .:/= ;MH/,    ,=/+%$XH@MM#@:
      -$##@+$###@H@MMM#######H:.    -/H#
 .,H@H@ X######@ -H#####@+-     -+H###@X
  .,@##H;      +XM##M/,     =%@###@X;-
X%-  :M##########$.    .:%M###@%:
M##H,   +H@@@$/-.  ,;$M###@%,          -
M####M=,,---,.-%%H####M$:          ,+@##
@##################@/.         :%H##@$-
M###############H,         ;HM##M$=
#################.    .=$M##M$=
################H..;XM##M$=          .:+
M###################@%=           =+@MH%
@#################M/.         =+H#X%=
=+M###############M,      ,/X#H+:,
  .;XM###########H=   ,/X#H+:;
     .=+HM#######M+/+HM@+=.
         ,:/%XM####H/.
              ,.:=-.
)",

    1 + R"(
       #+ @      # #              M#@
 .    .X  X.%##@;# #   +@#######X. @H%
   ,==.   ,######M+  -#####%M####M-    #
  :H##M%:=##+ .M##M,;#####/+#######% ,M#
 .M########=  =@#@.=#####M=M#######=  X#
 :@@MMM##M.  -##M.,#######M#######. =  M
             @##..###:.    .H####. @@ X,
   ############: ###,/####;  /##= @#. M
           ,M## ;##,@#M;/M#M  @# X#% X#
.%=   ######M## ##.M#:   ./#M ,M #M ,#$
##/         $## #+;#: #### ;#/ M M- @# :
#+ #M@MM###M-;M #:$#-##$H# .#X @ + $#. #
      ######/.: #%=# M#:MM./#.-#  @#: H#
+,.=   @###: /@ %#,@  ##@X #,-#@.##% .@#
#####+;/##/ @##  @#,+       /#M    . X,
   ;###M#@ M###H .#M-     ,##M  ;@@; ###
   .M#M##H ;####X ,@#######M/ -M###$  -H
    .M###%  X####H  .@@MM@;  ;@#M@
      H#M    /@####/      ,++.  / ==-,
               ,=/:, .+X@MMH@#H  #####$=
)",

// TODO: Add more graphics (optional)

};

enum class Color8Bit
{
    /*************************
    * Key
    * ===
    * Name            FG  BG
    * Black           30  40
    * Red             31  41
    * Green           32  42
    * Yellow          33  43
    * Blue            34  44
    * Magenta         35  45
    * Cyan            36  46
    * White           37  47
    * Bright Black    90  100
    * Bright Red      91  101
    * Bright Green    92  102
    * Bright Yellow   93  103 // TODO use background color
    * Bright Blue     94  104
    * Bright Magenta  95  105
    * Bright Cyan     96  106
    * Bright White    97  107
    **************************/

    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,

    Bright_Offset = 60,
    BG_Offset = 10,
};

Color8Bit operator|(Color8Bit l, Color8Bit r)
{
    return (Color8Bit)((int)l + (int)r);
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

// Overlay
const std::string g_overlay = 1 + R"ESC(
++====================================================================================================================++
||   _    _      _ _         __          __        _     _ _       || debug data  /                                   ||
||  | |  | |    | | |        \ \        / /       | |   | | |      ||===========-`                                    ||
||  | |__| | ___| | | ___     \ \  /\  / /__  _ __| | __| | |      || ~                                             < ||
||  |  __  |/ _ \ | |/ _ \     \ \/  \/ / _ \| '__| |/ _` | |      || ~                                             < ||
||  | |  | |  __/ | | (_) |     \  /\  / (_) | |  | | (_| |_|      || ~                                             < ||
||  |_|  |_|\___|_|_|\___( )     \/  \/ \___/|_|  |_|\__,_(_)      || ~                                             < ||
||_______________--------|/-------------------------------------.  || ~                                             < ||
|| communication \---------------------------------------------. `-||______________.._________________________________||
||===============-'                                             `--||_______________,-----------------------------------
|| >                                                           < ^ || pretty stuff /$                                  <
|| >                                                           < * ||============-`$                                   <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < * ||$                                                 <
|| >                                                           < v ||$                                                 <
++===================================================================$                                                 <)ESC";

constexpr size_t g_debugLineCount = 5;
constexpr size_t g_commLineCount = 19;
constexpr size_t g_PrettyLineCount = 19;

// Symbol used for positioning ancoring in overlay
enum class PosSymbol : char
{
    Comm = '>',
    Debug = '~',
    Pretty = '$',
};
struct LineData
{
    size_t pos, width;
    int x()
    {
        return (int)(pos % 121ull);
    }
    int y()
    {
        return (int)(pos / 121ull);
    }
};
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
    SetPrintPos(0, 0);
    ResetPrintColor();
    printf(g_overlay.c_str());
}

enum class DebugColor
{
    Msg,
    Warning,
    Critical,
};
struct DebugLine
{
    std::string m_str;
    DebugColor m_color = DebugColor::Msg;
};
DebugLine g_debugLines[g_debugLineCount]; // Limited set, get truncated if too long.

const char* g_DebugMessagePrefix[3]
{
    "Info: ",
    "Warning: ",
    "Critical: "
};
void SayDebug(DebugColor color, const char* fmt...)
{
    static int s_msgNumber = 0;
    for (int i = 4; i > 0; --i)
    {
        g_debugLines[i] = g_debugLines[i - 1];
    }
    {
        FILE* log = fopen(g_FileName_Log, "a+");
        
        fprintf_s(log, g_DebugMessagePrefix[(int)color]);
        va_list args;
        va_start(args, fmt);

        vfprintf_s(log, fmt, args);
        fclose(log);

        LineData data = FindPositionInOverlay(PosSymbol::Debug, line);
        PrepareLine(data);

        char numberInsert[sizeof("~                                             <")];
        sprintf_s(numberInsert, "[%#02i] %42s", ++s_msgNumber, );
        std::string _str = numberInsert + str;
        
        va_end(args);

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

std::stringstream g_commLines; // Unlimited stream, wrap if too long.
size_t g_commLineStreamLength;

void ClearComm()
{
    g_commLines.clear();
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

        auto [pos, space] = FindPositionInOverlay(PosSymbol::Comm, line);

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

void SayComm(const char* str)
{
    g_commLines << str;

    std::string line;
    ResetPrintColor(); // Color guard
    while (std::getline(g_commLines, line))
    {
        PrepareLine(FindPositionInOverlay(PosSymbol::Comm, line));
        const std::string& str = g_commLines[line];
        if (!str.empty()) printf(str.c_str());
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

#pragma endregion

enum class Type
{
    int_t,
    float_t,
    string_t,
    list_t,
    label_t,
};

enum class Instruction
{
    // 1 parameter
    lbl,    // Label <name>
    jmp,    // goto <name>

    // Functions
    KPrs,   // Simulate keypress <key>
    MClc,   // Simulate mouseclick <mousebutton>
    Prin,   // Prints the specified string
    Open,   // Opens the specified program (complicated)
    Prmp,   // Takes user input and jumps to the specified label
    Wait,   // Sleep <time>

    // 2 parameters
    var,    // Create a variable (<type> <name>)
    set,    // b = a

    // Functions
    Stor,   // userdata.ignore(INFINITY,a); userdata << <b>;
    Mous,   // Moves the mouse to the specified x and y coordinates (can also take an array[2] as input)

    // 3 parameters
    add,    // c = a + b
    sub,    // c = a - b
    mul,    // c = a * b
    div,    // c = a / b
    mod,    // c = a % b
};

const std::unordered_map<std::string, Instruction> g_Instructions
{
    { "lbl",    Instruction::lbl  },
    { "jmp",    Instruction::jmp  },

    { "KPrs",   Instruction::KPrs },
    { "MClc",   Instruction::MClc },
    { "Prin",   Instruction::Prin },
    { "Open",   Instruction::Open },
    { "Prmp",   Instruction::Prmp },
    { "Wait",   Instruction::Wait },

    { "var",    Instruction::var  },
    { "set",    Instruction::set  },

    { "Stor",   Instruction::Stor },
    { "Mous",   Instruction::Mous },

    { "add",    Instruction::add  },
    { "sub",    Instruction::sub  },
    { "mul",    Instruction::mul  },
    { "div",    Instruction::div  },
    { "mod",    Instruction::mod  },
};

int main()
{
    // Prepare log file
    {
        FILE* log;
        int sessionNumber;
        // Read
        log = fopen(g_FileName_Log, "r");
        fscanf_s(log, "Session No. %i", &sessionNumber);
        fclose(log);
        // Write
        log = fopen(g_FileName_Log, "w");
        time_t now = time(0);
        char dt[28];
        ctime_s(dt, sizeof(dt) - 1, &now);
        dt[27] = 0;
        ++sessionNumber;
        fprintf_s(log, "Session No. %i | %s\n", sessionNumber, dt);
        fclose(log);
    }

    SetPrintPos(0, 0);
    ResetPrintColor();
    printf(g_overlay.c_str());

    char* filename;
    {
        FILE* file = fopen(filename, "r"); // Open the file

        int versionNumber;
        fscanf_s(file, "version %i", &versionNumber);
        SayDebug(DebugColor::Msg, "Executing Virtual Hank Instruction Document %s using version number %i", filename, versionNumber);

        // TODO: Handle incompatible versions

        int lineNumber = 0;
        
        std::unordered_map<std::string, size_t> labelPositions; // name : position

        // TODO: Fill out labels map with seek positions
        while ()
        {
            char name[16];
            fscanf(file, "lbl %14s:", name, _countof(name));
            if (name) labelPositions[name] = ftell(file);
        }

        rewind(file);

        while (true)
        {
            std::string instruction;
            char _instruction[8];
            fscanf_s(file, "%7s", _instruction, _countof(_instruction));
            instruction = _instruction;
            auto it = g_Instructions.find(instruction);
            if (it != g_Instructions.end())
            {
                // TODO
                switch (it->second)
                {
                case Instruction::jmp:
                {
                    char label[16];
                    fscanf(file, "%14s;", label, _countof(name));
                    auto it = labelPositions.find(label);
                    if (it != labelPositions.end()) fseek(file, it->second, SEEK_SET);

                }   break;
                case Instruction::KPrs:
                    break;
                case Instruction::MClc:
                    break;
                case Instruction::Prin:
                    break;
                case Instruction::Open:
                    break;
                case Instruction::Prmp:
                    break;
                case Instruction::Wait:
                    break;
                case Instruction::var:
                    break;
                case Instruction::set:
                    break;
                case Instruction::Stor:
                    break;
                case Instruction::Mous:
                    break;
                case Instruction::add:
                    break;
                case Instruction::sub:
                    break;
                case Instruction::mul:
                    break;
                case Instruction::div:
                    break;
                case Instruction::mod:
                    break;
                default:
                    SayDebug(DebugColor::Critical, "");
                    break;
                }
            }

            auto it = instructionSet.find(instruction);
            if (it != instructionSet.end()) // Instruction is valid
            {

            }
            else
            {
                SayDebug(Error);
            }
        }

        while (std::getline(file, _line, ';')) // For each lineStream
        {
            ++lineNumber; // I want the lines to be { 1, 2, 3, ... } instead of { 0, 1, 2, ... }
            SayDebug("Now evaluating line " + std::to_string(lineNumber));
            std::stringstream lineStream(_line);
            {
                size_t start = _line.find_first_not_of(" \n\t");
                SayDebug("<[code]( " + _line.substr(start) + " )>");
            }

            std::string startingSymbol;
            lineStream >> startingSymbol;

            switch (SymbolType type = FindSymbolType(startingSymbol))
            {
            case SymbolType::Function:
            {
                auto it = g_FunctionSymbolList.find(startingSymbol);
                if (it != g_FunctionSymbolList.end())
                {
                    switch (it->second)
                    {
                    case FuncToken::Print:
                    {
                        size_t start = _line.find('\"');
                        start++;
                        size_t length = _line.find('\"', start) - start;
                        std::string str = _line.substr(start, length);
                        SayComm(str);
                        SayDebug(str);
                    }
                    break;

                    case FuncToken::Wait:
                    {
                        try
                        {
                            float time_ms = Param(lineStream);
                            Sleep((DWORD)time_ms);
                        }
                        catch (const char* msg)
                        {
                            SayDebug(msg, DebugColor::Critical);
                        }
                    }
                    break;

                    case FuncToken::MouseTo:
                        // TODO
                        break;
                    case FuncToken::Click:
                        // TODO
                        break;
                    case FuncToken::Keypress:
                        // TODO
                        break;
                    case FuncToken::Open:
                        // TODO
                        break;
                    }
                }
                else
                {
                    SayDebug("Function name not found in function list!", DebugColor::Critical);
                }
            }
            break;

            case SymbolType::Variable:
            {
                Variable(lineStream, startingSymbol); // We are almost certainly reassigning the variable if its name is at the start of a lineStream
            }
            break;

            case SymbolType::Keyword:
            {
                switch (g_Keywords.find(startingSymbol)->second)
                {
                case KeywordToken::var:
                    Variable(lineStream);
                    break;
                }
            }
            break;

            case SymbolType::Control:
            {
                Control(lineStream);
            }
            break;

            default: // Any literal type or otherwise unknown symbol
            {
                SayDebug("ERROR: INVALID SYNTAX! Couldn't find a definition!", DebugColor::Critical);
                goto END;
            }
            break;
            }

            //for (auto var : g_vars)
            //{
            //	SayDebug("Var " + var.first + " has value of " + std::to_string(var.second));
            //}
        }

    END:

        SayDebug("Reached end of document.");

        file.close();
    }

    

    SayDebug("Program started.", DebugColor::Msg);

    SayComm("Hi there! :D");
    SayComm("This is a really long line. I'm making this just to test how the GUI functions handle string overflow. I wrote the code so it can recursively wrap the text, prioritizing splitting at a space followed by breaking up words. It seems to be working pretty well :3");
    SayComm("At this point I'm kinda just tryina fill up the comm with text to make sure it acts correctly when filled. It should, hopefully, start producing a sort of \"scrolling\" effect as new lines are added below the existing ones, until the top messages are gone.");
    SayComm("Wow this has a lot more space than I thought");
    SayComm("Aaaa");
    SayComm("Words");
    SayComm("Words");
    SayComm("Words");
    SayComm("More words");
    SayComm("Even more words");
    SayComm("eeeeeeeee");
    SayComm("EEEEEEEEEEE");
    SayComm("AAAAAAAAA");

    SayPretty(g_AsciiGraphics[8]);

    InterpretFile("Test.vhid");

    SetPrintPos(0, 31);

    return 0;

    InterpretFile("Test.vhid");
    return 0;

    PrintRaw(versionNumber);
    PrintRaw(splashText);
    PrintRaw(screen1);

    PromptUser("ready", "to continue");

    PrintRaw(screen2);

    OpenExplorer();

    if (!AskUser("Would you like to run the program in manual mode?"))
    {
        POINT screenOffset = { 0,0 };
        SimulateMouseGoto(center + screenOffset);

        if (!AskUser("I've moved your mouse. Is your cursor on the same screen Intenet Explorer is open on?"))
        {
            screenOffset.x += 1920;
            SimulateMouseGoto(center + screenOffset);

            if (!AskUser("How about now?"))
            {
                std::cout << "You have so many monitors...";
                return 0;
            }
        }

        SimulateMouseClickAt(screenOffset + returnToClassic);
        Sleep(2000); // IE is slow
        SimulateMouseClickAt(screenOffset + library);
        Sleep(800);
        SimulateMouseClickAt(screenOffset + sync);
        Sleep(400);
        SimulateMouseGoto(screenOffset + allow);
        PrintRaw(explainAllow);

    }
    else PrintRaw(manualMode);

    std::cout << "\nMy work here is done.\n";

    PromptUser("quit", "to close the program");

    return 0;
}
