#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Step out of the "launcher" folder into the root "Code" directory
cd "$(dirname "$0")/.."

mkdir -p ./bin

# Find all .c files inside the launcher's local src directory
cFilenames=""
while IFS= read -r -d '' file; do
    cFilenames="$cFilenames $file"
done < <(find ./Launcher/src -name "*.c" -print0)

assembly="FractalEngine"

# Compiler flags for Clang executable:
# -g (debug symbols), -Wall (all warnings), -Werror (treat warnings as errors)
compilerFlags="-g -Wall -Werror -std=c23"

# Include launcher src, engine src, and Vulkan SDK paths
# Fixed to lowercase to match typical Unix directory structures
includeFlags="-I./Launcher/src -I./Engine/src -I${VULKAN_SDK}/include"
defines="-D_DEBUG -DFEXPORT"

# Unix-based linker flags:

if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS relative loader lookup token
    defines="$defines -D_DARWIN_C_SOURCE"
    linkerFlags="-L./bin -lFractal -Wl,-rpath,@loader_path/."
else
    # Linux relative loader lookup token
    defines="$defines -D_GNU_SOURCE"
    linkerFlags="-L./bin -lFractal -Wl,-rpath,'\$ORIGIN'"
fi
echo "Building ${assembly} Executable..."

if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS execution path
    clang $cFilenames $compilerFlags $defines $includeFlags -L./bin -lFractal -Wl,-rpath,@loader_path/. -o "./bin/${assembly}"
else
    # Linux execution path - Explicitly uses single quotes on the line call itself
    clang $cFilenames $compilerFlags $defines $includeFlags -L./bin -lFractal -Wl,-rpath,'$ORIGIN' -o "./bin/${assembly}"
fi

echo "Build successful: ./bin/${assembly}"