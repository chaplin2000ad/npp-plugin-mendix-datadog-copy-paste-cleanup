cmake_minimum_required(VERSION 3.10)

# --- Cross-Compilation Setup (MUST BE BEFORE project()) ---
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Project Name
project(NppPluginMendixDatadogCopyPasteCleanup)

# Define the shared library (DLL)
# Make sure src/main.cpp actually exists!
add_library(NppPluginMendixDatadogCopyPasteCleanup SHARED src/main.cpp)

# Npp Plugins need to start with specific defines
target_compile_definitions(NppPluginMendixDatadogCopyPasteCleanup PRIVATE UNICODE _UNICODE _WIN64)

# Link standard windows libraries
target_link_libraries(NppPluginMendixDatadogCopyPasteCleanup PRIVATE shlwapi)

# Strip debug symbols to reduce size (Optional)
add_custom_command(TARGET NppPluginMendixDatadogCopyPasteCleanup POST_BUILD
    COMMAND x86_64-w64-mingw32-strip $<TARGET_FILE:NppPluginMendixDatadogCopyPasteCleanup
>
)