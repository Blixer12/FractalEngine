#!/bin/bash

# Exit immediately if any command fails
set -e

echo "Building everything..."

# 1. Build the Engine
echo "----------------------------------------"
(
    cd Engine
    # Assumes your engine script is named build-engine.sh
    chmod +x Build.sh
    ./Build.sh
)

# 2. Build the Launcher
echo "----------------------------------------"
(
    cd Launcher
    # Assumes your launcher script is named build-launcher.sh
    chmod +x Build.sh
    ./Build.sh
)

echo "----------------------------------------"
echo "All assemblies built successfully."