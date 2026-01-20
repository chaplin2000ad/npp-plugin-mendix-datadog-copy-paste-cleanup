#include "PluginDefinition.h"
#include <stdio.h> 
#include <stdlib.h> // For malloc/free

// Standard Notepad++ Plugin Exports
#define DLLEXPORT extern "C" __declspec(dllexport)

// --- GLOBAL VARIABLES ---
NppData nppData; 
FuncItem funcItem[4];

// --- CONSTANTS ---
const int SCI_GETLINECOUNT       = 2154;
const int SCI_POSITIONFROMLINE   = 2167;
const int SCI_DELETERANGE        = 2645;
const int SCI_BEGINUNDOACTION    = 2078;
const int SCI_ENDUNDOACTION      = 2079;
const int SCI_LINELENGTH         = 2350;
const int SCI_GETLINE            = 2153;

// --- HELPER: Simple Debug Popup ---
void debugMsg(const TCHAR* msg) {
    ::MessageBox(NULL, msg, _T("Debug Trace"), MB_OK);
}

// --- YOUR CUSTOM FUNCTIONS ---

// Force __cdecl to match Notepad++ function pointer typedef
void __cdecl removeEmptyRows() {
    // debugMsg(_T("Step 1: Function Start"));

    // CHECK: Is the global data still valid?
    if (nppData._scintillaMain == NULL) {
        debugMsg(_T("CRITICAL ERROR: _scintillaMain is NULL. Global data was lost!"));
        return;
    }

    // DIRECT ACCESS: Skip the 'getCurrentScintilla' complexity for now.
    // We talk directly to the main editor window.
    HWND hSci = nppData._scintillaMain;
    
    // debugMsg(_T("Step 2: Handle Valid. Sending Undo Start..."));

    // 1. Group Undo
    ::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);
    // debugMsg(_T("Step 3: Undo Action Started. Getting Line Count..."));

    // 2. Get Line Count
    int lineCount = (int)::SendMessage(hSci, SCI_GETLINECOUNT, 0, 0);
    
    // Simple check to ensure we got a valid number
    if (lineCount <= 0) {
        debugMsg(_T("Step 4: Line count is 0 or invalid?"));
    } else {
    //    debugMsg(_T("Step 4: Line Count obtained. Starting Loop..."));
    }

    // 3. Iterate BACKWARDS
    bool deletedSomething = false;
    for (int i = lineCount - 1; i >= 0; i--) {
        int lineLength = (int)::SendMessage(hSci, SCI_LINELENGTH, i, 0);
        
        if (lineLength <= 0) continue;

        // Allocate memory (Pure C malloc)
        char* buffer = (char*)malloc(lineLength + 1);
        if (!buffer) {
            debugMsg(_T("Error: Malloc failed"));
            break;
        }

        // SCI_GETLINE fills the buffer
        ::SendMessage(hSci, SCI_GETLINE, i, (LPARAM)buffer);
        
        // Manual check for visible characters
        bool isBlank = true;
        for (int j = 0; j < lineLength; j++) {
            char c = buffer[j];
            if (c != ' ' && c != '\t' && c != '\r' && c != '\n' && c != '\0') {
                isBlank = false;
                break;
            }
        }
        
        free(buffer); // Release memory

        if (isBlank) {
            int lineStart = (int)::SendMessage(hSci, SCI_POSITIONFROMLINE, i, 0);
            ::SendMessage(hSci, SCI_DELETERANGE, lineStart, lineLength);
            deletedSomething = true;
        }
    }

    ::SendMessage(hSci, SCI_ENDUNDOACTION, 0, 0);
    
    if (deletedSomething) {
    //    debugMsg(_T("Step 5: Finished! (Rows were deleted)"));
    } else {
    //    debugMsg(_T("Step 5: Finished! (No empty rows found)"));
    }
}

// ... Placeholders ...
void __cdecl deduplicate() { ::MessageBox(NULL, _T("Deduplicate"), _T("Step 2"), MB_OK); }
void __cdecl compress() { ::MessageBox(NULL, _T("Compress"), _T("Step 3"), MB_OK); }
void __cdecl reorder() { ::MessageBox(NULL, _T("Reorder"), _T("Step 4"), MB_OK); }

// --- STANDARD PLUGIN BOILERPLATE ---
void commandMenuInit() {
    lstrcpy(funcItem[0]._itemName, _T("1. Remove Empty Rows"));
    funcItem[0]._pFunc = removeEmptyRows;
    funcItem[0]._init2Check = false;
    funcItem[0]._cmdID = 0;
    funcItem[0]._pShKey = NULL;

    lstrcpy(funcItem[1]._itemName, _T("2. Deduplicate"));
    funcItem[1]._pFunc = deduplicate;
    funcItem[1]._init2Check = false;
    funcItem[1]._cmdID = 1;
    funcItem[1]._pShKey = NULL;

    lstrcpy(funcItem[2]._itemName, _T("3. Compress"));
    funcItem[2]._pFunc = compress;
    funcItem[2]._init2Check = false;
    funcItem[2]._cmdID = 2;
    funcItem[2]._pShKey = NULL;

    lstrcpy(funcItem[3]._itemName, _T("4. Reorder"));
    funcItem[3]._pFunc = reorder;
    funcItem[3]._init2Check = false;
    funcItem[3]._cmdID = 3;
    funcItem[3]._pShKey = NULL;
}

DLLEXPORT BOOL isUnicode() { return TRUE; }

DLLEXPORT void setInfo(void * rawData) {
    // Manual copy of struct data
    NppData* pData = (NppData*)rawData;
    nppData = *pData;
    commandMenuInit();
}

DLLEXPORT const TCHAR * getName() { return _T("Mendix-Datadog Copy/Paste Cleanup"); }
DLLEXPORT LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) { return TRUE; }
DLLEXPORT void beNotified(void *notifyCode) { return; }
DLLEXPORT void * getFuncsArray(int *nbF) { *nbF = 4; return funcItem; }
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID lpReserved) { return TRUE; }