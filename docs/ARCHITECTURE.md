# unipm Architecture

## Overview

unipm is designed with a modular, three-layer architecture that separates concerns and ensures maintainability, extensibility, and safety.

```
┌─────────────────────────────────────────────────────────────┐
│                        User Input                           │
│                   unipm install docker                      │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                     PARSING LAYER                           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Parser: Command-line argument parsing               │   │
│  │ • Command type (install/remove/update/search)        │   │
│  │ • Package names                                      │   │
│  │ • Flags (--dry-run, --yes, --verbose, --pm)         │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Safety: Input validation & sanitization             │   │
│  │ • Package name validation                            │   │
│  │ • Shell injection prevention                         │   │
│  │ • Command sanitization                               │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                   DETECTION LAYER                           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ OS Detector: Identify platform & distribution        │   │
│  │ • Parse /etc/os-release (Linux)                      │   │
│  │ • Use uname (macOS)                                  │   │
│  │ • Windows API (Windows)                              │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ PM Detector: Find available package managers         │   │
│  │ • PATH probing (which/where)                         │   │
│  │ • Binary verification                                │   │
│  │ • Priority-based selection                           │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                  RESOLUTION LAYER                           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Config: Package knowledge base loading               │   │
│  │ • Load packages.json                                 │   │
│  │ • Merge user config                                  │   │
│  │ • Cache mappings                                     │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Resolver: Package name resolution                    │   │
│  │ • Exact match lookup                                 │   │
│  │ • Alias resolution                                   │   │
│  │ • Fuzzy matching (Levenshtein distance)              │   │
│  │ • Version specifier handling (e.g., "lts")           │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                    ADAPTER LAYER                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ PackageManagerAdapter (Abstract Interface)           │   │
│  │ • getInstallCommand()                                │   │
│  │ • getRemoveCommand()                                 │   │
│  │ • getUpdateCommand()                                 │   │
│  │ • requiresRoot()                                     │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                              │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │   APT    │ │  Pacman  │ │   Brew   │ │   DNF    │       │
│  │ Adapter  │ │ Adapter  │ │ Adapter  │ │ Adapter  │       │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘       │
│                                                              │
│  ┌──────────┐ ┌──────────┐                                  │
│  │  Winget  │ │  Choco   │                                  │
│  │ Adapter  │ │ Adapter  │                                  │
│  └──────────┘ └──────────┘                                  │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                   EXECUTION LAYER                           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ UI: User interaction                                 │   │
│  │ • Confirmation prompts                               │   │
│  │ • Command preview (dry-run)                          │   │
│  │ • Colorized output                                   │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Executor: Command execution                          │   │
│  │ • Privilege escalation (sudo)                        │   │
│  │ • Output capture                                     │   │
│  │ • Error handling                                     │   │
│  │ • Cross-platform process management                  │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Safety: Operation logging                            │   │
│  │ • Command history (~/.unipm/history.log)             │   │
│  │ • Timestamp & success status                         │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                    Package Manager                          │
│               (apt/pacman/brew/dnf/etc.)                    │
└─────────────────────────────────────────────────────────────┘
```

## Module Responsibilities

### Parser (`parser.cpp/h`)
- Parses command-line arguments into structured `Command` object
- Identifies command type (install, remove, update, search, etc.)
- Extracts package names and flags
- Validates command structure

### OS Detector (`os_detector.cpp/h`)
- Detects operating system type (Linux, macOS, Windows)
- Identifies Linux distribution (Ubuntu, Arch, Fedora, etc.)
- Parses `/etc/os-release` on Linux
- Uses platform-specific APIs for version detection

### PM Detector (`pm_detector.cpp/h`)
- Discovers available package managers on the system
- Probes PATH for package manager binaries
- Determines default package manager based on OS
- Retrieves package manager versions

### Config (`config.cpp/h`)
- Loads and parses `packages.json` database
- Manages package-to-PM mappings
- Supports user config overrides
- Caches package information

### Resolver (`resolver.cpp/h`)
- Resolves generic package names to PM-specific names
- Implements fuzzy matching using Levenshtein distance
- Handles package aliases
- Manages version specifiers (e.g., "node lts")
- Provides package suggestions

### Adapter (`adapter.cpp/h`)
- Abstract interface for package managers
- Concrete implementations for APT, Pacman, Brew, DNF, Winget, Chocolatey
- Generates PM-specific commands
- Indicates root privilege requirements

### Executor (`executor.cpp/h`)
- Executes shell commands
- Handles privilege escalation (sudo on Unix, admin on Windows)
- Captures stdout/stderr
- Cross-platform process management
- Dry-run mode support

### Safety (`safety.cpp/h`)
- Input sanitization and validation
- Shell injection prevention
- Package name validation
- Operation logging to history file

### UI (`ui.cpp/h`)
- Colorized terminal output
- User confirmation prompts
- Help and version information
- Result formatting

## Data Flow

### Example: `unipm install docker`

1. **Parser** parses command → `{type: INSTALL, packages: ["docker"]}`
2. **OS Detector** detects OS → `{type: LINUX, distro: UBUNTU}`
3. **PM Detector** finds package managers → `{type: APT, name: "apt"}`
4. **Config** loads `packages.json` → Package database in memory
5. **Resolver** resolves "docker" → `"docker.io"` for APT
6. **Adapter** generates command → `"apt install -y docker.io"`
7. **UI** shows preview → "Install docker.io via apt? [Y/n]"
8. **Executor** runs command → `sudo apt install -y docker.io`
9. **Safety** logs operation → `~/.unipm/history.log`

## Extension Points

### Adding New Package Manager

1. Create `src/adapters/newpm_adapter.cpp`
2. Implement `PackageManagerAdapter` interface
3. Add enum value to `PackageManager` in `types.h`
4. Update `AdapterFactory::create()` in `adapter.cpp`
5. Add detection logic in `pm_detector.cpp`
6. Update package mappings in `data/packages.json`

### Adding New Packages

Edit `data/packages.json`:
```json
{
  "packages": {
    "pkgname": {
      "aliases": ["alt1", "alt2"],
      "apt": "pkg-apt-name",
      "pacman": "pkg-pacman-name",
      "brew": "pkg-brew-name",
      "dnf": "pkg-dnf-name",
      "winget": "Publisher.PkgName",
      "choco": "pkg-choco-name"
    }
  }
}
```

## Security Considerations

1. **Input Validation**: All user input is validated before use
2. **Sanitization**: Package names are sanitized to prevent shell injection
3. **Confirmation Prompts**: Default behavior requires user confirmation
4. **Dry-Run Mode**: Users can preview commands before execution
5. **Operation Logging**: All commands are logged with timestamps
6. **Privilege Escalation**: Only used when necessary (PM requires root)

## Performance Characteristics

- **OS Detection**: O(1) file read + parsing
- **PM Detection**: O(n) where n = number of PMs to check (~6)
- **Package Resolution**: 
  - Exact match: O(1)
  - Fuzzy match: O(n·m) where n = number of packages, m = string length
- **Command Execution**: Depends on package manager

## Cross-Platform Considerations

### Linux
- Uses `/etc/os-release` for distro detection
- Relies on `sudo` for privilege escalation
- PATH probing via `which`

### macOS
- Uses `uname` for OS detection
- Homebrew typically doesn't require sudo
- PATH probing via `which`

### Windows
- Uses Windows API for version detection
- PATH probing via `where`
- Admin privileges via UAC elevation
- Process management via CreateProcess API

## Future Enhancements

1. **Snap & Flatpak Support**: Complete implementation
2. **Dependency Resolution**: Show package dependencies before installation
3. **Rollback**: Track installations for easy rollback
4. **Parallel Installations**: Install multiple packages concurrently
5. **Plugin System**: Allow third-party package manager adapters
6. **Configuration Profiles**: Save/load installation profiles
7. **Update Notifications**: Check for outdated packages
8. **Machine Learning**: Improve fuzzy matching with ML
