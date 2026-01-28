# Contributors

Thank you to everyone who has contributed to unipm!

## Core Team

- **IEEE-CS-BMSIT** - Creator & Lead Development

## How to Contribute

We welcome contributions from the community! Here's how you can help:

### Contributing Code

1. **Fork the repository**
2. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
3. **Make your changes**
4. **Test your changes** (`cd build && ctest`)
5. **Commit your changes** (`git commit -m 'Add amazing feature'`)
6. **Push to your branch** (`git push origin feature/amazing-feature`)
7. **Open a Pull Request**

### Adding Package Mappings

The easiest way to contribute is by adding new package mappings to `data/packages.json`:

```json
{
  "your-package": {
    "aliases": ["alternative-name"],
    "apt": "package-name-apt",
    "pacman": "package-name-pacman",
    "brew": "package-name-brew",
    "dnf": "package-name-dnf",
    "winget": "Publisher.PackageName",
    "choco": "package-name"
  }
}
```

Submit a PR with your additions!

### Reporting Bugs

Found a bug? Please [open an issue](https://github.com/ieee-cs-bmsit/unipm/issues/new) with:
- Description of the bug
- Steps to reproduce
- Expected vs actual behavior
- Your OS and package manager
- unipm version (`unipm --version`)

### Suggesting Features

Have an idea? [Open a feature request](https://github.com/ieee-cs-bmsit/unipm/issues/new) and describe:
- The problem you're trying to solve
- Your proposed solution
- Any alternatives you've considered

### Documentation

Improvements to documentation are always welcome:
- Fix typos
- Clarify instructions
- Add examples
- Translate to other languages

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## Recognition

All contributors will be recognized in this file and in our release notes!

---

**Special thanks to all our contributors! 🎉**

<!-- Contributors list will be auto-updated by all-contributors bot -->
