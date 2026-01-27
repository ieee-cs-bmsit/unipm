#!/bin/bash
# Build script for Unix systems (Linux, macOS)

set -e

echo "Building unipm..."

# Create build directory
mkdir -p build
cd build

# Run CMake
cmake ..

# Build
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Build complete! Binary location: build/bin/unipm"
echo ""
echo "To install system-wide, run:"
echo "  sudo make install"
