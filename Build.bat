@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist Build mkdir Build
if not exist Build\Assets\Shaders mkdir Build\Assets\Shaders

if "%1"=="Release" (
    echo Building RELEASE...
    SET config=Release
    SET defines=
    SET runtime=/MD
    SET opt=/O2
    SET GLFWdll=Release\glfw3.dll
    SET GLFWpath=%GLFW%\Lib\GLFW\lib-vc2022\Release
    SET GLFWlib=glfw3.lib
) else (
    echo Building DEBUG...
    SET config=Debug
    SET defines=/D DEBUG
    SET runtime=/MDd
    SET opt=/Od
    SET GLFWdll=Debug\glfw3d.dll
    SET GLFWpath=%GLFW%\Lib\GLFW\lib-vc2022\Debug
    SET GLFWlib=glfw3d.lib
)

SET includes=/Isrc /I%VULKAN_SDK%\Include /I%GLFW%\Include /I%GLM%
SET links=/link /LIBPATH:%VULKAN_SDK%\Lib /LIBPATH:%GLFWpath% vulkan-1.lib %GLFWlib% user32.lib gdi32.lib shell32.lib opengl32.lib advapi32.lib /ignore:4099

echo "Compiling main.cpp"

cl /EHsc /Z7 /W4 %runtime% %opt% /FeBuild\Engine_%config% %includes% %defines% src\*.cpp %links%

IF %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    exit /b %ERRORLEVEL%
)

echo Copying glfw3.dll...
copy /Y %GLFW%\Lib\GLFW\lib-vc2022\%GLFWdll% Build\

echo Compiling shaders...

for %%f in (Assets\Shaders\*.vert) do (
    %VULKAN_SDK%\Bin\glslc.exe %%f -o Build\Assets\Shaders\%%~nf.vert.spv
)

for %%f in (Assets\Shaders\*.frag) do (
    %VULKAN_SDK%\Bin\glslc.exe %%f -o Build\Assets\Shaders\%%~nf.frag.spv
)

echo Build complete.