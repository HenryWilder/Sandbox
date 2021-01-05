#pragma once
#include <windows.h>
#include <string.h>
#include <iostream>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>

void ClearScreen();

int StringLen(const char* string);

bool StringEquals(const char* input, const char* testAgainst, int strLen);
bool StringEquals(const char* input, const char* testAgainst);

bool PromptUser(const char* testFor, const char* message, bool clearAfter = true);

bool AskUser(const char* question, bool clearAfter = true);

void AssignLPSTR(LPWSTR& assignee, const WCHAR* str);

void OpenExplorer();


POINT GetMousePos();

void SimulateMouseMove(long x, long y);
void SimulateMouseMove(POINT p);

void SimulateMouseGoto(long x, long y);
void SimulateMouseGoto(POINT p);

void SimulateMouseClick();

void SimulateMouseClickAt(long x, long y);
void SimulateMouseClickAt(POINT p);
