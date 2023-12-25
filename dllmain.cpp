#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

FARPROC fpApiAddr[10];
HINSTANCE steamLib = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        uintptr_t moduleBase;
        moduleBase = (uintptr_t)GetModuleHandle(nullptr);

        TCHAR modulePath[MAX_PATH];
        GetModuleFileName(GetModuleHandle(nullptr), modulePath, MAX_PATH);

        TCHAR executableName[MAX_PATH];
        _tcslwr_s(modulePath);
        _tsplitpath_s(modulePath, nullptr, 0, nullptr, 0, executableName, ARRAYSIZE(executableName), nullptr, 0);

        // Only use stubs for CK versions < 1.6.1130
        if (_tcscmp(executableName, _T("creationkit")) == 0 && 
            _wcsicmp((const wchar_t*)(moduleBase + 0x5A86B90), L"1.6.1130.0"))
        {
            for (int i = 0; i < 10; ++i)
            {
                fpApiAddr[i] = (FARPROC)nullptr;
            }
        }
        else
        {
            const TCHAR* dllPath = _T("proxy\\steam_api64.dll");

            steamLib = LoadLibrary(dllPath);
            if (!steamLib)
                return FALSE;

            fpApiAddr[0] = GetProcAddress(steamLib, "SteamAPI_Init");
            fpApiAddr[1] = GetProcAddress(steamLib, "SteamAPI_GetHSteamUser");
            fpApiAddr[2] = GetProcAddress(steamLib, "SteamAPI_RegisterCallResult");
            fpApiAddr[3] = GetProcAddress(steamLib, "SteamAPI_RegisterCallback");
            fpApiAddr[4] = GetProcAddress(steamLib, "SteamAPI_RunCallbacks");
            fpApiAddr[5] = GetProcAddress(steamLib, "SteamAPI_UnregisterCallResult");
            fpApiAddr[6] = GetProcAddress(steamLib, "SteamAPI_UnregisterCallback");
            fpApiAddr[7] = GetProcAddress(steamLib, "SteamInternal_ContextInit");
            fpApiAddr[8] = GetProcAddress(steamLib, "SteamInternal_CreateInterface");
            fpApiAddr[9] = GetProcAddress(steamLib, "SteamInternal_FindOrCreateUserInterface");
        }
    }
    break;
    case DLL_PROCESS_DETACH:
        FreeLibrary(steamLib);
        break;
    }

    return TRUE;
}

#define DLLEXPORT extern "C" __declspec(dllexport)

extern "C"
{
    FARPROC PA = NULL;
    int RunASM();
}

// COMMON
DLLEXPORT BOOL SteamAPI_Init(char* param) 
{
    if (fpApiAddr[0])
        return (BOOL)fpApiAddr[0]();
    else 
        return true;
}

DLLEXPORT void SteamAPI_GetHSteamUser() { PA = fpApiAddr[1]; RunASM(); }
DLLEXPORT void SteamAPI_RegisterCallResult() { PA = fpApiAddr[2]; RunASM(); }
DLLEXPORT void SteamAPI_RegisterCallback() { PA = fpApiAddr[3]; RunASM(); }
DLLEXPORT void SteamAPI_RunCallbacks() { PA = fpApiAddr[4]; RunASM(); }
DLLEXPORT void SteamAPI_UnregisterCallResult() { PA = fpApiAddr[5]; RunASM(); }
DLLEXPORT void SteamAPI_UnregisterCallback() { PA = fpApiAddr[6]; RunASM(); }

// NEW INTERFACE
DLLEXPORT void SteamInternal_ContextInit() { PA = fpApiAddr[7]; RunASM(); }
DLLEXPORT void SteamInternal_CreateInterface() { PA = fpApiAddr[8]; RunASM(); }
DLLEXPORT void SteamInternal_FindOrCreateUserInterface() { PA = fpApiAddr[9]; RunASM(); }

// OLD INTERFACE
DLLEXPORT void* SteamClient() { return nullptr; }
DLLEXPORT void* SteamRemoteStorage() { return nullptr; }
DLLEXPORT void* SteamUser() { return nullptr; }
DLLEXPORT void* SteamUtils() { return nullptr; }
DLLEXPORT void* SteamApps() { return nullptr; };