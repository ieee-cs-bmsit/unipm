# Contributing to unipm

First off, thank you for considering contributing to unipm! It's people like you that make unipm such a great tool.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Pull Request Process](#pull-request-process)
- [Community](#community)

## Code of Conduct

This project and everyone participating in it is governed by the [unipm Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [atul.k.m@ieee.org](mailto:atul.k.m@ieee.org).

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/unipm.git
   cd unipm
   ```
   git remote add upstream https://github.com/ieee-cs-bmsit/unipm.git

## Development Setup

### Prerequisites

- **C++17 Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **CMake 3.15+**
- **Git**

### Setup Steps

1. **Download dependencies**:
   ```bash
   curl -L https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -o third_party/json.hpp
   ```

2. **Build**:
   ```bash
   # Linux/macOS
   ./scripts/build.sh
   
   # Windows
   .\scripts\build.ps1
   ```

3. **Run tests**:
   ```bash
   cd build
   ctest
   ```

See [SETUP.md](SETUP.md) for detailed setup instructions.

## How to Contribute

### Reporting Bugs

Before creating bug reports, please check the [existing issues](https://github.com/ieee-cs-bmsit/unipm/issues) to avoid duplicates.

When creating a bug report, use the **Bug Report** template and include:
- **Clear description** of the issue
- **Steps to reproduce** the behavior
- **Expected behavior**
- **Actual behavior**
- **Environment** (OS, version, package manager)
- **Logs/Screenshots** if applicable

### Suggesting Enhancements

Enhancement suggestions are tracked as [GitHub issues](https://github.com/ieee-cs-bmsit/unipm/issues). Use the **Feature Request** template and include:
- **Clear description** of the enhancement
- **Use case** and motivation
- **Proposed solution** (if you have one)
- **Alternatives** you've considered

### Adding New Package Mappings

The easiest way to contribute! Edit `data/packages.json`:

```json
{
  "packages": {
    "your-package": {
      "aliases": ["alternative-name"],
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

### Adding New Package Manager Support

1. **Create adapter** in `src/adapters/yourpm_adapter.cpp`:
   ```cpp
   class YourPMAdapter : public PackageManagerAdapter {
       // Implement interface methods
   };
   ```

2. **Update factory** in `src/adapters/choco_adapter.cpp`

3. **Add detection** in `src/pm_detector.cpp`

4. **Write tests** in `tests/`

5. **Update documentation**

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed architecture information.

## Coding Standards

### C++ Style

- **Standard**: C++17
- **Formatting**: Use `.clang-format` (run `clang-format -i <file>`)
- **Naming Conventions**:
  - Classes/Structs: `PascalCase` (e.g., `PackageManager`)
  - Functions/Variables: `camelCase` (e.g., `detectOS()`, `packageName`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_RETRIES`)
  - Files: `snake_case.cpp` (e.g., `os_detector.cpp`)

### Code Organization

- **Headers** in `include/unipm/`
- **Implementation** in `src/`
- **Tests** in `tests/`
- **One class per file** (generally)

### Best Practices

- **Use `const` where applicable**
- **RAII** for resource management
- **Prefer `std::string_view`** for read-only string parameters
- **Document public APIs** with comments
- **Avoid raw pointers**, use smart pointers
- **Error handling**: Return results or throw exceptions (be consistent)

### Comments

- **File headers**: Brief description of file purpose
- **Function comments**: Describe what, not how (code should be self-documenting)
- **Complex algorithms**: Explain the approach
- **TODOs**: Include your name and date (`// TODO(yourname): Fix this by 2026-02`)

Example:
```cpp
/**
 * Resolves a package name to a package-manager-specific name.
 * 
 * @param packageName Generic package name (e.g., "docker")
 * @param pm Target package manager
 * @param version Optional version specifier (e.g., "lts")
 * @return Resolved package with confidence score
 */
ResolvedPackage resolve(const std::string& packageName,
                       PackageManager pm,
                       const std::string& version = "");
```

## Testing

### Writing Tests

- **Unit tests** for individual modules (e.g., `test_resolver.cpp`)
- **Integration tests** for adapters
- **Test naming**: `test_<module>_<functionality>`

### Running Tests

```bash
cd build
ctest --verbose
```

### Test Requirements

- **All new features** must have tests
- **Bug fixes** should include regression tests
- **Aim for 70%+ code coverage**

## Pull Request Process

1. **Create a branch** from `main`:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**:
   - Follow coding standards
   - Write/update tests
   - Update documentation

3. **Commit your changes**:
   - Use [Conventional Commits](https://www.conventionalcommits.org/):
     - `feat: add winget support for Node.js LTS`
     - `fix: resolve memory leak in executor`
     - `docs: update README with new examples`
     - `test: add integration tests for brew adapter`
   - Keep commits atomic and focused

4. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

5. **Open a Pull Request**:
   - Use the PR template
   - Fill in all sections
   - Link related issues

6. **Address review feedback**:
   - Make requested changes
   - Push additional commits
   - Re-request review

7. **Merge**:
   - Maintainers will merge once approved
   - PR will be squashed into a single commit

### PR Checklist

Before submitting, ensure:

- [ ] Code follows the style guide
- [ ] Tests pass locally (`ctest`)
- [ ] New tests added for new functionality
- [ ] Documentation updated (README, ARCHITECTURE, etc.)
- [ ] No merge conflicts
- [ ] Commit messages follow Conventional Commits
- [ ] PR description is clear and complete

## Community

### Getting Help

- **Questions?** Open a [Discussion](https://github.com/ieee-cs-bmsit/unipm/discussions)
- **Found a bug?** Create an [Issue](https://github.com/ieee-cs-bmsit/unipm/issues)
- **Security issue?** See [SECURITY.md](SECURITY.md)

### Recognition

Contributors are recognized in our README using [all-contributors](https://allcontributors.org/). All types of contributions are valued!

### Communication

- **GitHub Issues**: Bug reports, feature requests
- **GitHub Discussions**: Questions, ideas, showcase
- **Email**: For security issues or private inquiries

---

Thank you for contributing to unipm! 🎉

**IEEE CS BMSIT&M CHAPTER**  
Maintained by Atul K M and contributors
