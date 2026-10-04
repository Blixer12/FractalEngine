REM Build script for engine
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (SET cFilenames=!cFilenames! %%f)

REM echo "Files:" %cFilenames%

SET assembly=Tests
SET compilerFlags=-g -Wno-missing-braces
REM -Wall -Werror -save-temps=obj -O0
SET includeFlags=-Isrc -I../Engine/src/
SET linkerFlags=-L../bin/ -lFractal.lib
SET defines=-D_DEBUG -DFIMPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%