#include <string>
#include <windows.h>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>
#include "Functions2.h"

#include "Debugging.h"

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