# UniPM Build Setup

## Build Requirements

UniPM requires a C++17 compiler and CMake 3.15 or later.

### Windows

You have **three options** for building on Windows:

#### Option 1: Visual Studio (Recommended for Windows)

1. **Install Visual Studio 2022** (or 2019/2017):
   - Download from https://visualstudio.microsoft.com/downloads/
   - During installation, select **"Desktop development with C++"** workload
   - This includes the MSVC compiler, CMake, and build tools

2. **Build from Developer Command Prompt**:
   ```powershell
   # Open "Developer Command Prompt for VS 2022" from Start Menu
   # Then navigate to your project:
   cd d:\unipm
   .\scripts\build.ps1
   ```

3. **Or build from regular PowerShell** (if Visual Studio is installed):
   ```powershell
   .\scripts\build.ps1
   ```

#### Option 2: Visual Studio Build Tools (Lighter Weight)

If you don't want the full Visual Studio IDE:

1. **Install Build Tools**:
   - Download from https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
   - Run the installer
   - Select **"C++ build tools"** workload
   - Install

2. **Build using the script**:
   ```powershell
   # Open "Developer Command Prompt for VS 2022" OR
   # Open PowerShell and run:
   .\scripts\build.ps1
   ```

#### Option 3: MinGW-w64 (GCC for Windows)

If you prefer GCC or can't install Visual Studio:

1. **Install MinGW-w64**:
   - Download from https://www.mingw-w64.org/ or use package managers:
     ```powershell
     # Using chocolatey
     choco install mingw

     # Using scoop
     scoop install mingw
     ```

2. **Use the MinGW build script**:
   ```powershell
   .\scripts\build-mingw.ps1
   ```

### Linux

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake

# Fedora/RHEL
sudo dnf install gcc-c++ cmake

# Arch
sudo pacman -S base-devel cmake

# Build
chmod +x scripts/build.sh
./scripts/build.sh
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Or install via Homebrew
brew install cmake

# Build
chmod +x scripts/build.sh
./scripts/build.sh
```

## Building

### Windows
```powershell
.\scripts\build.ps1
```

### Linux/macOS
```bash
./scripts/build.sh
```

## Troubleshooting

### Windows: "nmake not found" or "CMAKE_CXX_COMPILER not set"

**Problem**: You're seeing errors like:
```
CMake Error: CMAKE_CXX_COMPILER not set
Running 'nmake' '-?' failed with: no such file or directory
```

**Solutions**:

1. **Install Visual Studio** with C++ tools (see Option 1 above)
2. **Build from Developer Command Prompt** instead of regular PowerShell
3. **Or use MinGW** (see Option 3 above and use `build-mingw.ps1`)

### CMake not found

Install CMake from https://cmake.org/download/ or use a package manager:
```powershell
# Windows (chocolatey)
choco install cmake

# Windows (scoop)
scoop install cmake
```

### Tests subdirectory error

If you see "Cannot find source file: tests/CMakeLists.txt":
```powershell
# Create the tests directory structure
New-Item -ItemType Directory -Force -Path tests
```

## Manual Build

If the scripts don't work, you can build manually:

### Windows (Visual Studio)
```powershell
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

### Windows (MinGW)
```powershell
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
```

### Linux/macOS
```bash
mkdir build
cd build cmake ..
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

## Installation

After building, the binary will be located at:
- Windows: `build\bin\Release\unipm.exe`
- Linux/macOS: `build/bin/unipm`

To install system-wide:

### Windows
```powershell
# Copy to a directory in your PATH, e.g.:
Copy-Item build\bin\Release\unipm.exe C:\Windows\System32\
# Or add the build\bin\Release directory to your PATH
```

### Linux
```bash
sudo cp build/bin/unipm /usr/local/bin/
```

### macOS
```bash
sudo cp build/bin/unipm /usr/local/bin/
```

## Development

### Running Tests
```bash
cd build
ctest --output-on-failure
```

### Clean Build
```powershell
# Windows
Remove-Item -Recurse -Force build

# Linux/macOS
rm -rf build
```
