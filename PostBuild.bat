@echo off

REM Run from root directory!
if not exist "%cd%\bin\Assets\Shaders\" mkdir "%cd%\bin\Assets\Shaders"

echo "Compiling Shaders..."

echo "Assets/Shaders/Builtin.ObjectShader.vert.glsl -> bin/Assets/Shaders/Builtin.ObjectShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=vert Assets/Shaders/Builtin.ObjectShader.vert.glsl -o bin/Assets/Shaders/Builtin.ObjectShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Assets/Shaders/Builtin.ObjectShader.frag.glsl -> bin/Assets/Shaders/Builtin.ObjectShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=frag Assets/Shaders/Builtin.ObjectShader.frag.glsl -o bin/Assets/Shaders/Builtin.ObjectShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Copying Assets..."
echo xcopy "Assets" "bin\Assets" /h /i /c /k /e /r /y
xcopy "Assets" "bin\Assets" /h /i /c /k /e /r /y

echo "Done."