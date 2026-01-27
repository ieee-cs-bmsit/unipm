#include "unipm/types.h"

namespace unipm {

std::string osTypeToString(OSType type) {
    switch (type) {
        case OSType::LINUX: return "Linux";
        case OSType::MACOS: return "macOS";
        case OSType::WINDOWS: return "Windows";
        case OSType::UNKNOWN: return "Unknown";
    }
    return "Unknown";
}

std::string linuxDistroToString(LinuxDistro distro) {
    switch (distro) {
        case LinuxDistro::UBUNTU: return "Ubuntu";
        case LinuxDistro::DEBIAN: return "Debian";
        case LinuxDistro::ARCH: return "Arch Linux";
        case LinuxDistro::FEDORA: return "Fedora";
        case LinuxDistro::RHEL: return "Red Hat Enterprise Linux";
        case LinuxDistro::CENTOS: return "CentOS";
        case LinuxDistro::OPENSUSE: return "openSUSE";
        case LinuxDistro::GENTOO: return "Gentoo";
        case LinuxDistro::UNKNOWN: return "Unknown";
    }
    return "Unknown";
}

std::string packageManagerToString(PackageManager pm) {
    switch (pm) {
        case PackageManager::APT: return "apt";
        case PackageManager::PACMAN: return "pacman";
        case PackageManager::BREW: return "brew";
        case PackageManager::DNF: return "dnf";
        case PackageManager::YUM: return "yum";
        case PackageManager::WINGET: return "winget";
        case PackageManager::CHOCOLATEY: return "choco";
        case PackageManager::SNAP: return "snap";
        case PackageManager::FLATPAK: return "flatpak";
        case PackageManager::UNKNOWN: return "unknown";
    }
    return "unknown";
}

std::string commandTypeToString(CommandType cmd) {
    switch (cmd) {
        case CommandType::INSTALL: return "install";
        case CommandType::REMOVE: return "remove";
        case CommandType::UPDATE: return "update";
        case CommandType::SEARCH: return "search";
        case CommandType::LIST: return "list";
        case CommandType::INFO: return "info";
        case CommandType::HELP: return "help";
        case CommandType::VERSION: return "version";
    }
    return "unknown";
}

PackageManager stringToPackageManager(const std::string& str) {
    if (str == "apt") return PackageManager::APT;
    if (str == "pacman") return PackageManager::PACMAN;
    if (str == "brew") return PackageManager::BREW;
    if (str == "dnf") return PackageManager::DNF;
    if (str == "yum") return PackageManager::YUM;
    if (str == "winget") return PackageManager::WINGET;
    if (str == "choco" || str == "chocolatey") return PackageManager::CHOCOLATEY;
    if (str == "snap") return PackageManager::SNAP;
    if (str == "flatpak") return PackageManager::FLATPAK;
    return PackageManager::UNKNOWN;
}

} // namespace unipm
