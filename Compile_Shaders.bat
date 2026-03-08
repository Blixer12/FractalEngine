@echo off
REM -----------------------------
REM Compile all GLSL shaders to SPIR-V robustly
REM -----------------------------

REM Get the folder where this .bat file resides
SET "BASEDIR=%~dp0"

REM Create output folder if it doesn't exist
if not exist "%BASEDIR%Build\Assets\Shaders" (
    mkdir "%BASEDIR%Build\Assets\Shaders"
)

REM Compile all vertex shaders (*.vert)
for %%f in ("%BASEDIR%Assets\Shaders\*.vert") do (
    echo Compiling %%f
    "%VULKAN_SDK%\Bin\glslc.exe" "%%f" -o "%BASEDIR%Build\Assets\Shaders\%%~nf.vert.spv"
)

REM Compile all fragment shaders (*.frag)
for %%f in ("%BASEDIR%Assets\Shaders\*.frag") do (
    echo Compiling %%f
    "%VULKAN_SDK%\Bin\glslc.exe" "%%f" -o "%BASEDIR%Build\Assets\Shaders\%%~nf.frag.spv"
)

echo.
echo Shader compilation done!
pause