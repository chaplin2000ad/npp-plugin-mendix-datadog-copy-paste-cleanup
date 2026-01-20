#include <windows.h>
#include <tchar.h>

// Standard Notepad++ Plugin Exports
#define DLLEXPORT extern "C" __declspec(dllexport)

// Global handle to the Notepad++ window
HANDLE nppData = NULL;

// 1. Identification: Must be true for modern Npp
DLLEXPORT BOOL isUnicode() {
    return TRUE;
}

// 2. Initialization: Called when Npp starts
DLLEXPORT void setInfo(void *notUsed, void *hNppWnd) {
    nppData = hNppWnd;
}

// 3. Name: What shows up in the menu
DLLEXPORT const TCHAR * getName() {
    return _T("Docker Built Plugin");
}

// 4. Message Procedure: We can ignore this for now
DLLEXPORT LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) {
    return TRUE;
}

// 5. Notifications: React to Npp events (like saving a file)
DLLEXPORT void beNotified(void *notifyCode) {
    // Check struct headers for Scintilla notifications usually
    return;
}

// 6. Menu Commands: Define your menu items here
// (This requires a specific struct array, usually managed in a separate file, 
//  but we'll leave it empty for a successful compilation check)
DLLEXPORT void * getFuncsArray(int *nbF) {
    *nbF = 0; // 0 menu items for now
    return NULL;
}

// DLL Entry Point
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID lpReserved) {
    return TRUE;
}