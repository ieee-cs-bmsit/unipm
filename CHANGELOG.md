# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-01-27

### Added
- Initial release of unipm (Universal Package Manager)
- Cross-platform support for Linux, macOS, and Windows
- Automatic OS and distribution detection
- Package manager auto-detection (apt, pacman, brew, dnf, winget, chocolatey)
- Fuzzy package name matching using Levenshtein distance
- Package alias resolution from JSON database
- Command-line interface with install, remove, update, search, list, and info commands
- Safety features: dry-run mode, confirmation prompts, input sanitization
- Comprehensive package database with 30 common packages
- Support for version specifiers (e.g., "node lts")
- Operation logging to history file
- Colorized terminal output
- Unified syntax across all platforms

### Infrastructure
- GitHub Actions CI for automated testing on Linux, macOS, and Windows
- Issue and Pull Request templates
- Contributor Covenant Code of Conduct
- Security Policy with vulnerability reporting workflow
- Code quality configuration (.clang-format, .clang-tidy)

### Security
- Input validation and sanitization
- Shell injection prevention
- Confirmation prompts for destructive operations
- Operation logging with timestamps

### Documentation
- Comprehensive README with usage examples
- Architecture documentation
- Build scripts for Unix and Windows
- Unit tests for core modules
