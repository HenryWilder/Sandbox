#include <windows.h>
#include <string.h>
#include <iostream>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>

void ClearScreen()
{
    system("CLS");
    std::cout << "\x1b[1;1H";
}

int StringLen(const char* string)
{
    int length = 0;
    while (string[length]) {
        ++length;
    }
    return length;
}

bool StringEquals(const char* input, const char* testAgainst, int strLen)
{
    for (int c = 0; c < strLen; ++c)
    {
        if (input[c] != testAgainst[c]) return false;
    }
    return true;
}

bool PromptUser(const char* testFor, const char* message, bool clearAfter = true)
{
    int strLen = StringLen(testFor);
    std::cout << "\n(Type \"" << testFor << "\" and press enter " << message << ")\n";
    std::string input;
    std::cin >> input;
    input[strLen] = NULL;
    bool out = (input == testFor);
    if (clearAfter) ClearScreen();
    return out;
}

bool AskUser(const char* question, bool clearAfter = true)
{
    std::cout << "\n" << question << " (yes/no)\n";

    std::string response;
    std::cin >> response;

    if (response == "yes" || response == "Yes" || response == "YES" || response == "y" || response == "Y")
    {
        if (clearAfter) ClearScreen();
        return true;
    }
    else if (response == "no" || response == "No" || response == "NO" || response == "n" || response == "N")
    {
        if (clearAfter) ClearScreen();
        return false;
    }
    else
    {
        std::cout << "\nI'm sorry, I'm not sure I understand... What was that?\n";
        return AskUser(question);
    }
}

/*
static LPARAM CreateLParam(long LoWord, long HiWord)
{
    return (LPARAM)((HiWord << 16) | (LoWord & 0xffff));
}
*/

void AssignLPSTR(LPWSTR& assignee, const WCHAR* str)
{
    size_t c;
    for (c = 0; c < MAX_PATH; ++c)
    {
        if (*(str + c))
        {
            assignee[c] = (WCHAR)*(str + c);
        }
        else break;
    }
    for (c; c < MAX_PATH; ++c)
    {
        assignee[c] = NULL;
    }
}

void OpenExplorer()
{
    //HWND window = 0;
    //ShellExecute(window, L"open", L"https://lachmanstuartcom.sharepoint.com/sites/LACHMANSTUART/Shared%20Documents/Forms/AllItems.aspx?RootFolder=%2Fsites%2FLACHMANSTUART%2FShared%20Documents", 0, 0, SW_MAXIMIZE);

    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    LPWSTR commandline = new WCHAR[MAX_PATH];
    AssignLPSTR(commandline,
        (L"C:\\\\Program Files\\Internet Explorer\\iexplore.exe https://lachmanstuartcom.sharepoint.com/sites/LACHMANSTUART/Shared%20Documents/Forms/AllItems.aspx"));

    if (CreateProcess(
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
    ))
    {
        delete [] commandline;
        //WaitForSingleObject(processInfo.hProcess, INFINITE);

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }
    else
    {
        delete [] commandline;
        int response = MessageBox(GetConsoleWindow(), L"The process could not be started...", L"Oops", 5L | 30L);
        if (response == IDRETRY) OpenExplorer();
    }
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
void SimulateMouseMove(POINT p) { SimulateMouseMove(p.x, p.y); }

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
void SimulateMouseGoto(POINT p) { SimulateMouseGoto(p.x, p.y); }

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

void SimulateMouseClickAt(long x, long y)
{
    SimulateMouseGoto(x, y);
    SimulateMouseClick();
}
void SimulateMouseClickAt(POINT p) { SimulateMouseClickAt(p.x, p.y); }


const char* versionNumber = R"TXT(Version 0.00.1)TXT";

const char* splashText =
R"TXT(
   _    _      _ _         __          __        _     _ _   
  | |  | |    | | |        \ \        / /       | |   | | |  
  | |__| | ___| | | ___     \ \  /\  / /__  _ __| | __| | |  
  |  __  |/ _ \ | |/ _ \     \ \/  \/ / _ \| '__| |/ _` | |  
  | |  | |  __/ | | (_) |     \  /\  / (_) | |  | | (_| |_|  
  |_|  |_|\___|_|_|\___( )     \/  \/ \___/|_|  |_|\__,_(_)  
                       |/                                    
)TXT";

const char* screen1 =
R"TXT(Hi! My name is VirtualHank! I'm here to help you with tech issues. :)
Unfortunately, because I am only a program, I have some trouble with certain things you may find trivial.
For this reason, I ask that you kindly keep my window open, so that I communicate to you when I need help.

I would also like to let you know ahead of time: I may perform hardware simulation.
I will try my best not to take away control from you. I hope you are able to trust me.
It will make it easier for me to help you if you can trust me to perform my programming.

In case I do scare you at any point, or you feel you need to force-quit me but can't press my close button,
I am programmed to return full control to you if you slam your cursor into any edge/corner of the screen.

Once you've finished reading, press enter and I will begin.
Do not worry. The first step will not have any scary mouse-movement whatsoever. And you have have the option
to put me into manual mode.)TXT";

const char* screen2 = 
R"TXT(For security reasons, I need you to sign in yourself.
I could have been coded to access your sign-in data on my own using WTSQueryUserToken(),
but that would have risked a security leak if not used properly. Letting you sign in is much safer.

It would also be greatly appriciated if you would please maximize the Internet Explorer window.
My programming is rather rigid, and I have trouble seeing. I can perform the actions needed to do this for you, but
to know where the buttons to click are, I need IE to be maximized. Otherwise I may accidentally click the wrong spot.

Let me know when you're ready to proceed!
If you'd like to do this manually, type "manual" before pressing enter, and I'll tell you the steps :))TXT";

const char* manualMode = 
R"TXT(STEPS FOR SYNCING SHAREPOINT
============================
0. In the bottom-left corner of your Internet Explorer window, you should see a button/link that says
   "Return to classic SharePoint"
   Click it.

1. Once the page finishes loading, click on the "LIBRARY" tab/button in the top-left of the window.
   It will be in the light-gray bar, and to the right of the "FILES" tab/button.

2. This will unroll an actions panel. On that panel, there will be a "Sync" button. Click it.

3. A message box will appear in the center of your screen. Press "Allow".
   The message box is simply making sure you want to sync SharePoint.

If you've completed all these steps, SharePoint should be syncing!)TXT";

const char* explainAllow =
R"TXT(So you know, I have the ability to click Allow right now.
I care about making sure you don't feel as though I'm taking away control from you, though.
If you want to sync SharePoint, you may press "Allow" yourself, now.)TXT";

void PrintRaw(const char* string) { std::cout << string << std::endl; }

constexpr POINT center = { 960, 540 };
constexpr POINT returnToClassic = { 90, 1013 };
constexpr POINT library = { 150, 150 };
constexpr POINT sync = { 600, 200 };
constexpr POINT allow = { 1000, 570 };

POINT operator+(const POINT& p1, const POINT& p2)
{
    return { p1.x + p2.x, p1.y + p2.y };
}

int main()
{
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