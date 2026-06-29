#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Step out of the "engine" folder into the root "Code" directory
cd "$(dirname "$0")/.."

# Find all .c files inside the engine's local src directory
cFilenames=""
while IFS= read -r -d '' file; do
    cFilenames="$cFilenames $file"
done < <(find ./Engine/src -name "*.c" -print0)

assembly="Fractal"

# Compiler flags for Clang: 
# -g (debug symbols), -shared (build DLL/shared library), -fPIC (position-independent code, required for shared libs)
compilerFlags="-g -shared -fPIC -Wall -Werror -std=c23"

# Tell the compiler to look in the engine/src directory for internal headers
includeFlags="-I./Engine/src -I${VULKAN_SDK}/include -I../vcpkg_installed/x64-linux/include"
defines="-D_DEBUG -DFEXPORT"

if [[ "$OSTYPE" == "darwin"* ]]; then

    defines="$defines -D_DARWIN_C_SOURCE"
    outputFile="./bin/lib${assembly}.dylib"
    # macOS-specific dynamic library linking adjustments
    linkerFlags="-L${VULKAN_SDK}/lib -lvulkan -dynamiclib -Wl,-install_name,@rpath/lib${assembly}.dylib"
else

    defines="$defines -D_GNU_SOURCE"
    outputFile="./bin/lib${assembly}.so"
    # Linux system library links
    linkerFlags="-L${VULKAN_SDK}/lib -lvulkan -lxcb -lxkbcommon -lxkbcommon-x11"
fi

echo "Building ${assembly} Shared Library..."
clang $cFilenames $compilerFlags $defines $includeFlags $linkerFlags -o "$outputFile"

echo "Build successful: $outputFile"