@echo off

echo "Compiling Shaders..."

echo "Assets/Shaders/Builtin.MaterialShader.vert.glsl -> Assets/Shaders/Builtin.MaterialShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=vert Assets/Shaders/Builtin.MaterialShader.vert.glsl -o Assets/Shaders/Builtin.MaterialShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Assets/Shaders/Builtin.MaterialShader.frag.glsl -> Assets/Shaders/Builtin.MaterialShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=frag Assets/Shaders/Builtin.MaterialShader.frag.glsl -o Assets/Shaders/Builtin.MaterialShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)



echo "Assets/Shaders/Builtin.UIShader.vert.glsl -> Assets/Shaders/Builtin.UIShader.vert.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=vert Assets/Shaders/Builtin.UIShader.vert.glsl -o Assets/Shaders/Builtin.UIShader.vert.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Assets/Shaders/Builtin.UIShader.frag.glsl -> Assets/Shaders/Builtin.UIShader.frag.spv"
%VULKAN_SDK%\bin\glslc.exe --target-env=vulkan1.3 -fshader-stage=frag Assets/Shaders/Builtin.UIShader.frag.glsl -o Assets/Shaders/Builtin.UIShader.frag.spv
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)


echo "Done."