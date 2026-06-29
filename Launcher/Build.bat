@ECHO OFF
SetLocal EnableDelayedExpansion

REM --- Detect and initialize MSVC Environment if 'cl' is missing ---
WHERE cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    ECHO MSVC Build Tools not detected in PATH. Initializing environment...
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

REM /MD builds a standard console application executable
SET compilerFlags=/Zi /MD /W4 /WX /TC /std:clatest

REM Crucial: Include engine\src so the Launcher can read the engine's API headers!
SET includeFlags=/ILauncher\src /IEngine\src /I"%VULKAN_SDK%\Include"
SET defines=/D_DEBUG /D_CRT_SECURE_NO_WARNINGS /DFEXPORT

REM Links directly to the compiled ./bin/fractal.lib file from the engine build
SET linkerFlags=/link /LIBPATH:./bin/ Fractal.lib /OUT:"./bin/%assembly%.exe"

ECHO "Building %assembly% Executable..."
CALL cl %cFilenames% %compilerFlags% %defines% %includeFlags% %linkerFlags%

IF %ERRORLEVEL% NEQ 0 ( EXIT /B %ERRORLEVEL% )