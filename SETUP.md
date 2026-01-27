# unipm Setup Instructions

## Quick Setup

### Prerequisites

1. **C++17 Compiler**
   - Linux: GCC 7+ or Clang 5+
   - macOS: Xcode 10+ or Apple Clang
   - Windows: Visual Studio 2017+ or MinGW-w64

2. **CMake 3.15 or higher**

3. **Git** (for cloning)

### Installation Steps

#### 1. Clone the Repository

```bash
git clone https://github.com/ieee-cs-bmsit/unipm.git
cd unipm
```

#### 2. Download Dependencies

Download the nlohmann/json header:

```bash
# Linux/macOS
curl -L https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -o third_party/json.hpp

# Windows PowerShell
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp" -OutFile "third_party\json.hpp"
```

#### 3. Build

**Linux/macOS:**
```bash
chmod +x scripts/build.sh
./scripts/build.sh
```

**Windows:**
```powershell
.\scripts\build.ps1
```

#### 4. Install (Optional)

**Linux/macOS:**
```bash
cd build
sudo make install
```

This installs:
- Binary: `/usr/local/bin/unipm`
- Data: `/usr/local/share/unipm/packages.json`

**Windows:**
Copy `build\bin\Release\unipm.exe` to a directory in your PATH, such as:
- `C:\Windows\System32` (requires admin)
- `C:\Program Files\unipm\` (create directory and add to PATH)

Also copy `data\packages.json` to `C:\Program Files\unipm\`

#### 5. Test

```bash
cd build
ctest
```

Or run the binary directly:
```bash
# Linux/macOS
./bin/unipm --version

# Windows
.\bin\Release\unipm.exe --version
```

## Development Setup

### Project Structure

```
unipm/
├── CMakeLists.txt          # Root CMake configuration
├── README.md               # Project documentation
├── LICENSE                 # MIT license
├── CHANGELOG.md            # Version history
├── include/                # Header files
│   └── unipm/
│       ├── types.h
│       ├── os_detector.h
│       ├── pm_detector.h
│       ├── parser.h
│       ├── resolver.h
│       ├── config.h
│       ├── adapter.h
│       ├── executor.h
│       ├── safety.h
│       └── ui.h
├── src/                    # Implementation files
│   ├── main.cpp
│   ├── types.cpp
│   ├── os_detector.cpp
│   ├── pm_detector.cpp
│   ├── parser.cpp
│   ├── resolver.cpp
│   ├── config.cpp
│   ├── executor.cpp
│   ├── safety.cpp
│   ├── ui.cpp
│   └── adapters/
│       ├── apt_adapter.cpp
│       ├── pacman_adapter.cpp
│       ├── brew_adapter.cpp
│       ├── dnf_adapter.cpp
│       ├── winget_adapter.cpp
│       └── choco_adapter.cpp
├── data/                   # Package database
│   └── packages.json
├── third_party/            # External dependencies
│   ├── json.hpp           # nlohmann/json (download separately)
│   └── README.md
├── tests/                  # Unit tests
│   ├── CMakeLists.txt
│   ├── test_os_detector.cpp
│   └── test_resolver.cpp
├── scripts/                # Build scripts
│   ├── build.sh           # Unix build script
│   └── build.ps1          # Windows build script
└── docs/                   # Documentation
    └── ARCHITECTURE.md
```

### Building from Source

#### Debug Build

```bash
mkdir -p build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

#### Release Build

```bash
mkdir -p build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Running Tests

```bash
cd build
ctest --verbose
```

Or run individual tests:
```bash
./bin/test_os_detector
./bin/test_resolver
```

## Extending unipm

### Adding New Packages

Edit `data/packages.json`:

```json
{
  "packages": {
    "your-package": {
      "aliases": ["alternative-name", "another-alias"],
      "apt": "package-name-debian",
      "pacman": "package-name-arch",
      "brew": "package-name-homebrew",
      "dnf": "package-name-fedora",
      "winget": "Publisher.PackageName",
      "choco": "package-name-chocolatey"
    }
  }
}
```

### Adding a New Package Manager

1. **Add enum value** in `include/unipm/types.h`:
   ```cpp
   enum class PackageManager {
       // ... existing values ...
       YOUR_PM,
   };
   ```

2. **Create adapter** in `src/adapters/yourpm_adapter.cpp`:
   ```cpp
   class YourPMAdapter : public PackageManagerAdapter {
       // Implement interface methods
   };
   ```

3. **Update factory** in `src/adapters/choco_adapter.cpp`:
   ```cpp
   std::unique_ptr<PackageManagerAdapter> AdapterFactory::create(PackageManager pm) {
       // Add case for YOUR_PM
   }
   ```

4. **Add detection** in `src/pm_detector.cpp`

## Troubleshooting

### Build Errors

**Missing json.hpp:**
```
fatal error: third_party/json.hpp: No such file or directory
```
Solution: Download nlohmann/json as described in step 2 above.

**CMake version too old:**
```
CMake 3.15 or higher is required
```
Solution: Update CMake or use a newer version.

### Runtime Issues

**Package database not found:**
```
Could not load package database
```
Solution: Ensure `data/packages.json` is in the correct location:
- Development: Run from project root
- Installed: `/usr/local/share/unipm/packages.json` (Unix) or `C:\Program Files\unipm\packages.json` (Windows)

**No package manager found:**
```
No package manager found on this system
```
Solution: Install at least one supported package manager (apt, pacman, brew, dnf, winget, or chocolatey).

## Getting Help

- Check the [README](../README.md) for usage examples
- Review [ARCHITECTURE.md](../docs/ARCHITECTURE.md) for design details
- Open an issue on GitHub for bugs or feature requests
