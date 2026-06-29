@ECHO OFF
SetLocal EnableDelayedExpansion

REM --- Detect and initialize MSVC Environment if 'cl' is missing ---
WHERE cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    ECHO MSVC Build Tools not detected in PATH. Initializing environment...
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
SET compilerFlags=/Zi /LD /W4 /WX /TC /std:clatest

REM Tell the compiler to look in the engine/src directory for internal headers
SET includeFlags=/IEngine\src /I"%VULKAN_SDK%\Include" /I"../vcpkg_installed/x64-windows/include"
SET defines=/D_DEBUG /DFEXPORT /D_CRT_SECURE_NO_WARNINGS
SET linkerFlags=/link /LIBPATH:"%VULKAN_SDK%\Lib" /LIBPATH:"../vcpkg_installed/x64-windows/lib" user32.lib vulkan-1.lib /OUT:"./bin/%assembly%.dll" /IMPLIB:"./bin/%assembly%.lib"

ECHO "Building %assembly% DLL..."
CALL cl %cFilenames% %compilerFlags% %defines% %includeFlags% %linkerFlags%

IF %ERRORLEVEL% NEQ 0 ( EXIT /B %ERRORLEVEL% )