@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (SET cFilenames=!cFilenames! "%%f")

SET assembly=Tests
SET compilerFlags=/Zi /W3 /WX-
REM /Wall /WX /O0
SET includeFlags=/Isrc /I..\Engine\src\
SET linkerFlags=/link /LIBPATH:..\bin\ Fractal.lib
SET defines=/D_DEBUG /DFIMPORT

ECHO Building %assembly%...
clang-cl %cFilenames% %compilerFlags% /Fe:..\bin\%assembly%.exe %defines% %includeFlags% %linkerFlags%