#include "PluginDefinition.h"
#include <stdio.h> 
#include <stdlib.h> // For malloc/free
#include <string.h> // For strcmp
#include <ctype.h>  // For isdigit, isalpha

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
const int SCI_GETLINEENDPOSITION = 2136;
const int SCI_REPLACETARGET      = 2194;
const int SCI_SETTARGETSTART     = 2190;
const int SCI_SETTARGETEND       = 2192;

// --- HELPER: Simple Debug Popup ---
void debugMsg(const TCHAR* msg) {
    ::MessageBox(NULL, msg, _T("Debug Trace"), MB_OK);
}

// --- HELPER: Timestamp Detector ---
// Format: "Jan 16 16:38:52.152" (19 chars)
// We check the pattern: Mmm DD HH:MM:SS.mmm
bool isTimestamp(const char* s, int len) {
    if (len < 19) return false;

    // Check fixed separators first (fastest fail)
    if (s[3] != ' ') return false;
    if (s[6] != ' ') return false;
    if (s[9] != ':') return false;
    if (s[12] != ':') return false;
    if (s[15] != '.') return false;

    // Check Month (3 letters)
    if (!isalpha(s[0]) || !isalpha(s[1]) || !isalpha(s[2])) return false;

    // Check Digits (Day, Time)
    // We allow spaces for single digit days e.g. "Jan  6" if that occurs, 
    // but strict "Jan 16" implies digits. Let's assume strict digits for now.
    if (!isdigit(s[4]) || !isdigit(s[5])) return false; 
    if (!isdigit(s[7]) || !isdigit(s[8])) return false;
    if (!isdigit(s[10]) || !isdigit(s[11])) return false;
    if (!isdigit(s[13]) || !isdigit(s[14])) return false;
    if (!isdigit(s[16]) || !isdigit(s[17]) || !isdigit(s[18])) return false;

    return true;
}

// --- CUSTOM FUNCTIONS ---

// 1. REMOVE EMPTY ROWS
void __cdecl removeEmptyRows() {
    if (nppData._scintillaMain == NULL) return;
    HWND hSci = nppData._scintillaMain;

    ::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);

    int lineCount = (int)::SendMessage(hSci, SCI_GETLINECOUNT, 0, 0);
    
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

// 2. DEDUPLICATE
void __cdecl deduplicate() {
    if (nppData._scintillaMain == NULL) return;
    HWND hSci = nppData._scintillaMain;

    ::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);

    int lineCount = (int)::SendMessage(hSci, SCI_GETLINECOUNT, 0, 0);

    for (int i = lineCount - 1; i > 0; i--) {
        int lenCurrent = (int)::SendMessage(hSci, SCI_LINELENGTH, i, 0);
        int lenPrev    = (int)::SendMessage(hSci, SCI_LINELENGTH, i - 1, 0);

        if (lenCurrent != lenPrev) continue;
        if (lenCurrent <= 0) continue; 

        char* bufCurrent = (char*)malloc(lenCurrent + 1);
        char* bufPrev    = (char*)malloc(lenPrev + 1);

        if (!bufCurrent || !bufPrev) {
            if (bufCurrent) free(bufCurrent);
            if (bufPrev) free(bufPrev);
            break; 
        }

        ::SendMessage(hSci, SCI_GETLINE, i, (LPARAM)bufCurrent);
        bufCurrent[lenCurrent] = '\0'; 
        ::SendMessage(hSci, SCI_GETLINE, i - 1, (LPARAM)bufPrev);
        bufPrev[lenPrev] = '\0'; 

        if (strcmp(bufCurrent, bufPrev) == 0) {
            int lineStart = (int)::SendMessage(hSci, SCI_POSITIONFROMLINE, i, 0);
            ::SendMessage(hSci, SCI_DELETERANGE, lineStart, lenCurrent);
        }

        free(bufCurrent);
        free(bufPrev);
    }
    ::SendMessage(hSci, SCI_ENDUNDOACTION, 0, 0);
}

// 3. COMPRESS
void __cdecl compress() {
    if (nppData._scintillaMain == NULL) return;
    HWND hSci = nppData._scintillaMain;

    ::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);

    // Loop backwards from Last Line down to Line 1 (stopping before 0)
    // We check if Line[i] is a timestamp. If NOT, we merge it into Line[i-1].
    int lineCount = (int)::SendMessage(hSci, SCI_GETLINECOUNT, 0, 0);

    for (int i = lineCount - 1; i > 0; i--) {
        
        // 1. Get the content of the CURRENT line (i)
        int len = (int)::SendMessage(hSci, SCI_LINELENGTH, i, 0);
        if (len <= 0) continue;

        char* buffer = (char*)malloc(len + 1);
        if (!buffer) break;

        ::SendMessage(hSci, SCI_GETLINE, i, (LPARAM)buffer);
        // Note: buffer contains the text + \r\n usually
        
        // 2. Check if it is a NEW record (Timestamp)
        bool isNewRecord = isTimestamp(buffer, len);
        free(buffer);

        // 3. If it is NOT a new record, merge it up!
        if (!isNewRecord) {
            // We want to replace the "Gap" (Newline) between line i-1 and line i with a SPACE.
            
            // End of text on previous line (excludes \r\n)
            int prevLineEnd = (int)::SendMessage(hSci, SCI_GETLINEENDPOSITION, i - 1, 0);
            
            // Start of current line
            int currLineStart = (int)::SendMessage(hSci, SCI_POSITIONFROMLINE, i, 0);

            // The gap is the newline characters
            // Safety: Ensure gap exists
            if (currLineStart > prevLineEnd) {
                ::SendMessage(hSci, SCI_SETTARGETSTART, prevLineEnd, 0);
                ::SendMessage(hSci, SCI_SETTARGETEND, currLineStart, 0);
                ::SendMessage(hSci, SCI_REPLACETARGET, 1, (LPARAM)" ");
            }
        }
    }

    ::SendMessage(hSci, SCI_ENDUNDOACTION, 0, 0);
}

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