#include "PluginDefinition.h"
#include <stdio.h> 
#include <stdlib.h> // For malloc/free
#include <string.h> // For strcmp

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

// --- CUSTOM FUNCTIONS ---

// 1. REMOVE EMPTY ROWS
void __cdecl removeEmptyRows() {
    if (nppData._scintillaMain == NULL) return;
    HWND hSci = nppData._scintillaMain;

    ::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);

    int lineCount = (int)::SendMessage(hSci, SCI_GETLINECOUNT, 0, 0);
    
    // Iterate BACKWARDS
    for (int i = lineCount - 1; i >= 0; i--) {
        int lineLength = (int)::SendMessage(hSci, SCI_LINELENGTH, i, 0);
        if (lineLength <= 0) continue;

        char* buffer = (char*)malloc(lineLength + 1);
        if (!buffer) break;

        ::SendMessage(hSci, SCI_GETLINE, i, (LPARAM)buffer);
        
        bool isBlank = true;
        for (int j = 0; j < lineLength; j++) {
            char c = buffer[j];
            if (c != ' ' && c != '\t' && c != '\r' && c != '\n' && c != '\0') {
                isBlank = false;
                break;
            }
        }
        free(buffer);

        if (isBlank) {
            int lineStart = (int)::SendMessage(hSci, SCI_POSITIONFROMLINE, i, 0);
            ::SendMessage(hSci, SCI_DELETERANGE, lineStart, lineLength);
        }
    }

    ::SendMessage(hSci, SCI_ENDUNDOACTION, 0, 0);
}

// 2. DEDUPLICATE (Consecutive Only)
void __cdecl deduplicate() {
    if (nppData._scintillaMain == NULL) return;
    HWND hSci = nppData._scintillaMain;

    ::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);

    int lineCount = (int)::SendMessage(hSci, SCI_GETLINECOUNT, 0, 0);

    // Iterate BACKWARDS from the last line down to the 2nd line (index 1)
    // We compare Line[i] with Line[i-1]
    for (int i = lineCount - 1; i > 0; i--) {
        
        int lenCurrent = (int)::SendMessage(hSci, SCI_LINELENGTH, i, 0);
        int lenPrev    = (int)::SendMessage(hSci, SCI_LINELENGTH, i - 1, 0);

        // Optimization: If lengths differ, they can't be duplicates
        if (lenCurrent != lenPrev) continue;
        if (lenCurrent <= 0) continue; 

        // Allocate buffers
        char* bufCurrent = (char*)malloc(lenCurrent + 1);
        char* bufPrev    = (char*)malloc(lenPrev + 1);

        if (!bufCurrent || !bufPrev) {
            // Memory Panic: Clean up and abort safely
            if (bufCurrent) free(bufCurrent);
            if (bufPrev) free(bufPrev);
            break; 
        }

        // Fetch Line Content
        ::SendMessage(hSci, SCI_GETLINE, i, (LPARAM)bufCurrent);
        bufCurrent[lenCurrent] = '\0'; // IMPORTANT: SCI_GETLINE does not null-terminate

        ::SendMessage(hSci, SCI_GETLINE, i - 1, (LPARAM)bufPrev);
        bufPrev[lenPrev] = '\0'; 

        // Compare
        if (strcmp(bufCurrent, bufPrev) == 0) {
            // Identical! Delete the CURRENT line (the bottom one)
            int lineStart = (int)::SendMessage(hSci, SCI_POSITIONFROMLINE, i, 0);
            ::SendMessage(hSci, SCI_DELETERANGE, lineStart, lenCurrent);
        }

        free(bufCurrent);
        free(bufPrev);
    }

    ::SendMessage(hSci, SCI_ENDUNDOACTION, 0, 0);
}

// ... Placeholders for future steps ...
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
    NppData* pData = (NppData*)rawData;
    nppData = *pData;
    commandMenuInit();
}

DLLEXPORT const TCHAR * getName() { return _T("Mendix-Datadog Copy/Paste Cleanup"); }
DLLEXPORT LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) { return TRUE; }
DLLEXPORT void beNotified(void *notifyCode) { return; }
DLLEXPORT void * getFuncsArray(int *nbF) { *nbF = 4; return funcItem; }
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID lpReserved) { return TRUE; }