// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

Win::BOOL APIENTRY DllMain( Win::HMODULE hModule,
                       Win::DWORD  ul_reason_for_call,
                       Win::LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

NotWin::Model SetTexture(NotWin::Model model, NotWin::Texture2D texture)
{
    return model; // TODO
}
