#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

#include <windows.h>
#include <tchar.h>

// 1. Define the Shortcut Key Structure (Required by Npp)
struct ShortcutKey {
    bool _isCtrl;
    bool _isAlt;
    bool _isShift;
    UCHAR _key;
};

// 2. Define the structure for a Menu Item
typedef void (__cdecl * PFUNCPLUGINCMD)();

struct FuncItem {
    TCHAR   _itemName[64];      // The name you see in the menu
    PFUNCPLUGINCMD _pFunc;      // The function to run when clicked
    int     _cmdID;             // Internal ID (usually 0, 1, 2...)
    bool    _init2Check;        // Should it have a checkmark?
    ShortcutKey * _pShKey;      // <--- CHANGED: Must be a POINTER to ShortcutKey
};

// 3. Define the structure for Notepad++ Data (handles)
struct NppData {
    HWND _nppHandle;        // Handle to the main Notepad++ window
    HWND _scintillaMain;    // Handle to the main text area
    HWND _scintillaSecond;  // Handle to the secondary text area (if split view)
};

#endif // PLUGINDEFINITION_H