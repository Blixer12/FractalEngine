#!/bin/bash

# Run from root directory!
mkdir -p bin/Assets
mkdir -p bin/Assets/Shaders

echo "Compiling Shaders..."

echo "Assets/Shaders/Builtin.MaterialShader.vert.glsl -> bin/Assets/Shaders/Builtin.MaterialShader.vert.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=vert Assets/Shaders/Builtin.MaterialShader.vert.glsl -o bin/Assets/Shaders/Builtin.MaterialShader.vert.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Assets/Shaders/Builtin.MaterialShader.frag.glsl -> bin/Assets/Shaders/Builtin.MaterialShader.frag.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=frag Assets/Shaders/Builtin.MaterialShader.frag.glsl -o bin/Assets/Shaders/Builtin.MaterialShader.frag.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Copying Assets..."
echo cp -R "Assets" "bin"
cp -R "Assets" "bin"

echo "Done."