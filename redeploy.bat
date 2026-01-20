@echo off
setlocal

:: 1. Handle UNC Paths (Fixes "CMD.EXE was started with...")
:: This temporarily maps the network path to a drive letter so CMD works correctly.
pushd "%~dp0"

:: 2. Load Configuration
set "SETTINGS_FILE=local.settings.bat"

if exist "%SETTINGS_FILE%" (
    call "%SETTINGS_FILE%"
) else (
    echo [ERROR] Configuration file 'local.settings.bat' not found!
    echo Please create it with WSL_SOURCE and NPP_PLUGIN_DIR variables.
    pause
    goto :Cleanup
)

:: Validate variables were actually set
if "%NPP_PLUGIN_DIR%"=="" (
    echo [ERROR] NPP_PLUGIN_DIR is not set. Check local.settings.bat.
    pause
    goto :Cleanup
)

set "DEST_FILE=%NPP_PLUGIN_DIR%\NppPluginMendixDatadogCopyPasteCleanup.dll"

:: 3. Execution Logic
echo [1/4] Killing Notepad++...
taskkill /F /IM notepad++.exe >nul 2>&1
timeout /t 1 /nobreak >nul

echo [2/4] Ensuring plugin directory exists...
if not exist "%NPP_PLUGIN_DIR%" (
    mkdir "%NPP_PLUGIN_DIR%"
)

echo [3/4] Copying new DLL...
copy /Y "%WSL_SOURCE%" "%DEST_FILE%"

echo [4/4] Unblocking DLL (Removing Mark of the Web)...
powershell -Command "Unblock-File -LiteralPath '%DEST_FILE%'"

echo Done! Restarting Notepad++...
start "" "C:\Program Files\Notepad++\notepad++.exe"

:Cleanup
popd