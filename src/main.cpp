#include "PluginDefinition.h"

// Standard Notepad++ Plugin Exports
#define DLLEXPORT extern "C" __declspec(dllexport)

// Global Variables
HANDLE nppData = NULL;          // Handle to Notepad++
FuncItem funcItem[1];           // Array for our menu items (We have 1)

// --- YOUR CUSTOM FUNCTIONS ---

// The actual function that runs when you click the menu item
void runCleanup() {
    // Placeholder: Just show a pop-up for now
    ::MessageBox(NULL, _T("Cleanup logic goes here!"), _T("Mendix-Datadog Plugin"), MB_OK);
}

// --- STANDARD PLUGIN BOILERPLATE ---

// 1. Initialize the Menu (Called internally by us)
void commandMenuInit() {
    // Define the 1st Menu Item
    lstrcpy(funcItem[0]._itemName, _T("Run Cleanup")); // Text in the sub-menu
    funcItem[0]._pFunc = runCleanup;                   // Function to call
    funcItem[0]._init2Check = false;                   // No checkmark
    funcItem[0]._cmdID = 0;                            // ID 0
    funcItem[0]._shKey = NULL;                         // No shortcut key
}

// 2. Identification
DLLEXPORT BOOL isUnicode() {
    return TRUE;
}

// 3. Initialization: Called when Npp starts
DLLEXPORT void setInfo(void *notUsed, void *hNppWnd) {
    nppData = hNppWnd;
    commandMenuInit(); // Set up our menu structure
}

// 4. Name: The Top-Level Plugin Name
DLLEXPORT const TCHAR * getName() {
    return _T("Mendix-Datadog Copy/Paste Cleanup");
}

// 5. Message Procedure
DLLEXPORT LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) {
    return TRUE;
}

// 6. Notifications
DLLEXPORT void beNotified(void *notifyCode) {
    return;
}

// 7. Export the Menu Items to Notepad++
DLLEXPORT void * getFuncsArray(int *nbF) {
    *nbF = 1; // Tell Npp we have 1 menu item
    return funcItem;
}

// DLL Entry Point
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID lpReserved) {
    return TRUE;
}