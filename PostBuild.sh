#!/bin/bash

echo "Compiling Shaders..."

echo "Assets/Shaders/Builtin.MaterialShader.vert.glsl -> Assets/Shaders/Builtin.MaterialShader.vert.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=vert Assets/Shaders/Builtin.MaterialShader.vert.glsl -o Assets/Shaders/Builtin.MaterialShader.vert.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Assets/Shaders/Builtin.MaterialShader.frag.glsl -> Assets/Shaders/Builtin.MaterialShader.frag.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=frag Assets/Shaders/Builtin.MaterialShader.frag.glsl -o Assets/Shaders/Builtin.MaterialShader.frag.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi



echo "Assets/Shaders/Builtin.UIShader.vert.glsl -> Assets/Shaders/Builtin.UIShader.vert.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=vert Assets/Shaders/Builtin.UIShader.vert.glsl -o Assets/Shaders/Builtin.UIShader.vert.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Assets/Shaders/Builtin.UIShader.frag.glsl -> Assets/Shaders/Builtin.UIShader.frag.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=frag Assets/Shaders/Builtin.UIShader.frag.glsl -o Assets/Shaders/Builtin.UIShader.frag.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Done."