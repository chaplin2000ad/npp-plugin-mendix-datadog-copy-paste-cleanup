@echo off
echo Killing Notepad++...
taskkill /F /IM notepad++.exe >nul 2>&1

:: Wait a moment for the file lock to release
timeout /t 1 /nobreak >nul

echo Copying new DLL...
:: UPDATE THESE PATHS TO MATCH YOUR COMPUTER
set SOURCE="\\wsl.localhost\Debian\home\night-shift-dad\source\repos\npp-plugin-mendix-datadog-copy-paste-cleanup\build\NppPluginMendixDatadogCopyPasteCleanup.dll"
set DEST="C:\Program Files\Notepad++\plugins\NppPluginMendixDatadogCopyPasteCleanup\NppPluginMendixDatadogCopyPasteCleanup.dll"

copy /Y %SOURCE% %DEST%

echo Restarting Notepad++...
start "" "C:\Program Files\Notepad++\notepad++.exe"