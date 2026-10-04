#!/bin/bash

# Exit immediately if any command fails
set -e

echo "Building everything..."

# 1. Build the Engine
echo "----------------------------------------"
make -f Makefile.Engine.Linux.mak

# 2. Build the Launcher
echo "----------------------------------------"
make -f Makefile.Launcher.Linux.mak

# 3. Build the Tests
echo "----------------------------------------"
make -f Makefile.Tests.Linux.mak

echo "----------------------------------------"
echo "All assemblies built successfully."