cmake_minimum_required(VERSION 3.10)

# --- Cross-Compilation Setup ---
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Project Name
project(NppPluginMendixDatadogCopyPasteCleanup)

# Define the shared library (DLL)
add_library(NppPluginMendixDatadogCopyPasteCleanup SHARED src/main.cpp)

# --- FIX 1: Remove the "lib" prefix ---
set_target_properties(NppPluginMendixDatadogCopyPasteCleanup PROPERTIES PREFIX "")

# Npp Plugins need to start with specific defines
target_compile_definitions(NppPluginMendixDatadogCopyPasteCleanup PRIVATE UNICODE _UNICODE _WIN64)

# --- FIX 2: Static Linking (Required for MinGW plugins to load) ---
target_link_libraries(NppPluginMendixDatadogCopyPasteCleanup 
    PRIVATE 
    shlwapi
    -static
    -static-libgcc
    -static-libstdc++
)

#Strip debug symbols (Optional - Uncomment if you fixed the syntax error)
add_custom_command(TARGET NppPluginMendixDatadogCopyPasteCleanup POST_BUILD
   COMMAND x86_64-w64-mingw32-strip $<TARGET_FILE:NppPluginMendixDatadogCopyPasteCleanup>
)