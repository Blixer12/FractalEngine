@ECHO OFF
SetLocal EnableDelayedExpansion

REM --- Detect and initialize environment for clang-cl ---
WHERE clang-cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    ECHO clang-cl not detected in PATH. Initializing Visual Studio build environment...
    CALL "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

REM Step out of the "engine" folder into the root "Code" directory
CD /D "%~dp0"..\

REM Find all .c files inside the engine's local src directory
SET cFilenames=
FOR /R ./engine/src %%f in (*.c) do (
    SET cFilenames=!cFilenames! "%%f"
)

SET assembly=Fractal
SET compilerFlags=/Zi /LD /W4 /TC /WX /std:clatest

REM Include paths and definitions
SET includeFlags=/IEngine\src /I"%VULKAN_SDK%\Include" /I"../vcpkg_installed/x64-windows/include"
SET defines=/D_DEBUG /DFEXPORT /D_CRT_SECURE_NO_WARNINGS
SET linkerFlags=/link /LIBPATH:"%VULKAN_SDK%\Lib" /LIBPATH:"../vcpkg_installed/x64-windows/lib" user32.lib vulkan-1.lib /OUT:"./bin/%assembly%.dll" /IMPLIB:"./bin/%assembly%.lib"

ECHO "Building %assembly% DLL with clang-cl..."
CALL clang-cl %cFilenames% %compilerFlags% %defines% %includeFlags% %linkerFlags%

IF %ERRORLEVEL% NEQ 0 ( EXIT /B %ERRORLEVEL% )