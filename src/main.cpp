#include "PluginDefinition.h"

// Standard Notepad++ Plugin Exports
#define DLLEXPORT extern "C" __declspec(dllexport)

// Global Variables
HANDLE nppData = NULL;          // Handle to Notepad++
FuncItem funcItem[4];           // Array for our 4 menu items

// --- HELPER: SCINTILLA ACCESS ---
// We will use this later to talk to the text editor
const int SCI_GETTEXT = 2182;
const int SCI_SETTEXT = 2181;
const int SCI_GETTEXTLENGTH = 2183;

// --- YOUR CUSTOM FUNCTIONS ---

void removeEmptyRows() {
    ::MessageBox(NULL, _T("Logic for 'Remove Empty Rows' goes here."), _T("Step 1"), MB_OK);
}

void deduplicate() {
    ::MessageBox(NULL, _T("Logic for 'Deduplicate' goes here."), _T("Step 2"), MB_OK);
}

void compress() {
    ::MessageBox(NULL, _T("Logic for 'Compress' goes here."), _T("Step 3"), MB_OK);
}

void reorder() {
    ::MessageBox(NULL, _T("Logic for 'Reorder' goes here."), _T("Step 4"), MB_OK);
}

// --- STANDARD PLUGIN BOILERPLATE ---

// 1. Initialize the Menu (Called internally by us)
void commandMenuInit() {
    // 1. Remove Empty Rows
    lstrcpy(funcItem[0]._itemName, _T("1. Remove Empty Rows"));
    funcItem[0]._pFunc = removeEmptyRows;
    funcItem[0]._init2Check = false;
    funcItem[0]._cmdID = 0;
    funcItem[0]._pShKey = NULL;  // <--- FIXED: _pShKey

    // 2. Deduplicate
    lstrcpy(funcItem[1]._itemName, _T("2. Deduplicate"));
    funcItem[1]._pFunc = deduplicate;
    funcItem[1]._init2Check = false;
    funcItem[1]._cmdID = 1;
    funcItem[1]._pShKey = NULL;  // <--- FIXED: _pShKey

    // 3. Compress
    lstrcpy(funcItem[2]._itemName, _T("3. Compress"));
    funcItem[2]._pFunc = compress;
    funcItem[2]._init2Check = false;
    funcItem[2]._cmdID = 2;
    funcItem[2]._pShKey = NULL;  // <--- FIXED: _pShKey

    // 4. Reorder
    lstrcpy(funcItem[3]._itemName, _T("4. Reorder"));
    funcItem[3]._pFunc = reorder;
    funcItem[3]._init2Check = false;
    funcItem[3]._cmdID = 3;
    funcItem[3]._pShKey = NULL;  // <--- FIXED: _pShKey
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
    *nbF = 4; // Tell Npp we now have 4 menu items
    return funcItem;
}

// DLL Entry Point
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID lpReserved) {
    return TRUE;
}