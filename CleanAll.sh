#!/bin/bash

# Exit immediately if any command fails
set -e

echo "Cleaning all build artifacts..."

# 1. Clean the Engine
echo "----------------------------------------"
make -f Makefile.Engine.Linux.mak clean

# 2. Clean the Launcher
echo "----------------------------------------"
make -f Makefile.Launcher.Linux.mak clean

# 3. Clean the Tests
echo "----------------------------------------"
make -f Makefile.Tests.Linux.mak clean

echo "----------------------------------------"
echo "All assemblies cleaned successfully."