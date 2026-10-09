@echo off

REM Run from root directory!
if not exist "%cd%\bin\Assets\Shaders\" mkdir "%cd%\bin\Assets\Shaders"

echo "Compiling Shaders..."

echo "Assets/Shaders/Builtin.MaterialShader.vert.glsl -> bin/Assets/Shaders/Builtin.MaterialShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=vert Assets/Shaders/Builtin.MaterialShader.vert.glsl -o bin/Assets/Shaders/Builtin.MaterialShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Assets/Shaders/Builtin.MaterialShader.frag.glsl -> bin/Assets/Shaders/Builtin.MaterialShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=frag Assets/Shaders/Builtin.MaterialShader.frag.glsl -o bin/Assets/Shaders/Builtin.MaterialShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying Assets..."
echo xcopy "Assets" "bin\Assets" /h /i /c /k /e /r /y
xcopy "Assets" "bin\Assets" /h /i /c /k /e /r /y

echo "Done."