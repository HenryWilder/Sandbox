<<<<<<< HEAD
#include "Functions.h"

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
bool StringEquals(const char* input, const char* testAgainst)
{
    StringEquals(input, testAgainst, StringLen(testAgainst));
}


bool PromptUser(const char* testFor, const char* message, bool clearAfter)
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

bool AskUser(const char* question, bool clearAfter)
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

void AssignLPSTR(LPWSTR& assignee, const WCHAR* str)
{
    size_t c;
    for (c = 0; c < MAX_PATH; ++c)
    {
        if (*(str + c))
        {
            assignee[c] = (WCHAR) * (str + c);
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
        delete[] commandline;
        //WaitForSingleObject(processInfo.hProcess, INFINITE);

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }
    else
    {
        delete[] commandline;
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
=======
#include "Functions.h"

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
bool StringEquals(const char* input, const char* testAgainst)
{
    StringEquals(input, testAgainst, StringLen(testAgainst));
}


bool PromptUser(const char* testFor, const char* message, bool clearAfter)
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

bool AskUser(const char* question, bool clearAfter)
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

void AssignLPSTR(LPWSTR& assignee, const WCHAR* str)
{
    size_t c;
    for (c = 0; c < MAX_PATH; ++c)
    {
        if (*(str + c))
        {
            assignee[c] = (WCHAR) * (str + c);
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
        delete[] commandline;
        //WaitForSingleObject(processInfo.hProcess, INFINITE);

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }
    else
    {
        delete[] commandline;
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
>>>>>>> 0641cdd506e8bd139e5868b54fb5c1396d32d3b8
