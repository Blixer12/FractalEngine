@ECHO OFF
SetLocal EnableDelayedExpansion

REM --- Detect and initialize environment for clang-cl ---
WHERE clang-cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    ECHO clang-cl not detected in PATH. Initializing Visual Studio build environment...
    CALL "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

REM Step out of the "launcher" folder into the root "Code" directory
CD /D "%~dp0"..\

REM Find all .c files inside the launcher's local src directory
SET cFilenames=
FOR /R ./launcher/src %%f in (*.c) do (
    SET cFilenames=!cFilenames! "%%f"
)

SET assembly=FractalEngine
SET compilerFlags=/Zi /MD /W4 /TC /WX /std:clatest

REM Include paths and definitions
SET includeFlags=/ILauncher\src /IEngine\src /I"%VULKAN_SDK%\Include"
SET defines=/D_DEBUG /D_CRT_SECURE_NO_WARNINGS /DFEXPORT

REM Link directly against engine DLL import library
SET linkerFlags=/link /LIBPATH:./bin/ Fractal.lib /OUT:"./bin/%assembly%.exe"

ECHO "Building %assembly% Executable with clang-cl..."
CALL clang-cl %cFilenames% %compilerFlags% %defines% %includeFlags% %linkerFlags%

IF %ERRORLEVEL% NEQ 0 ( EXIT /B %ERRORLEVEL% )