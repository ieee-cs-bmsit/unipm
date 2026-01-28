#include "unipm/pm_detector.h"
#include <cstdlib>
#include <cstdio>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace unipm {

std::vector<PMInfo> PMDetector::detectAll() {
    std::vector<PMInfo> pms;
    
    // Try to detect each package manager
    PMInfo apt = detectAPT();
    if (apt.type != PackageManager::UNKNOWN) pms.push_back(apt);
    
    PMInfo pacman = detectPacman();
    if (pacman.type != PackageManager::UNKNOWN) pms.push_back(pacman);
    
    PMInfo brew = detectBrew();
    if (brew.type != PackageManager::UNKNOWN) pms.push_back(brew);
    
    PMInfo dnf = detectDNF();
    if (dnf.type != PackageManager::UNKNOWN) pms.push_back(dnf);
    
    PMInfo yum = detectYUM();
    if (yum.type != PackageManager::UNKNOWN) pms.push_back(yum);
    
    PMInfo winget = detectWinget();
    if (winget.type != PackageManager::UNKNOWN) pms.push_back(winget);
    
    PMInfo choco = detectChocolatey();
    if (choco.type != PackageManager::UNKNOWN) pms.push_back(choco);
    
    PMInfo snap = detectSnap();
    if (snap.type != PackageManager::UNKNOWN) pms.push_back(snap);
    
    PMInfo flatpak = detectFlatpak();
    if (flatpak.type != PackageManager::UNKNOWN) pms.push_back(flatpak);
    
    return pms;
}

PMInfo PMDetector::detectDefault(const OSInfo& osInfo) {
    // Priority based on OS
    if (osInfo.type == OSType::LINUX) {
        switch (osInfo.distro) {
            case LinuxDistro::UBUNTU:
            case LinuxDistro::DEBIAN:
                if (isAvailable(PackageManager::APT)) return detectAPT();
                break;
            case LinuxDistro::ARCH:
                if (isAvailable(PackageManager::PACMAN)) return detectPacman();
                break;
            case LinuxDistro::FEDORA:
            case LinuxDistro::RHEL:
            case LinuxDistro::CENTOS:
                if (isAvailable(PackageManager::DNF)) return detectDNF();
                if (isAvailable(PackageManager::YUM)) return detectYUM();
                break;
            default:
                break;
        }
    } else if (osInfo.type == OSType::MACOS) {
        if (isAvailable(PackageManager::BREW)) return detectBrew();
    } else if (osInfo.type == OSType::WINDOWS) {
        if (isAvailable(PackageManager::WINGET)) return detectWinget();
        if (isAvailable(PackageManager::CHOCOLATEY)) return detectChocolatey();
    }
    
    // Fallback: return first available
    auto pms = detectAll();
    if (!pms.empty()) {
        return pms[0];
    }
    
    PMInfo unknown;
    unknown.type = PackageManager::UNKNOWN;
    return unknown;
}

bool PMDetector::isAvailable(PackageManager pm) {
    switch (pm) {
        case PackageManager::APT: return checkBinary("apt");
        case PackageManager::PACMAN: return checkBinary("pacman");
        case PackageManager::BREW: return checkBinary("brew");
        case PackageManager::DNF: return checkBinary("dnf");
        case PackageManager::YUM: return checkBinary("yum");
        case PackageManager::WINGET: return checkBinary("winget");
        case PackageManager::CHOCOLATEY: return checkBinary("choco");
        case PackageManager::SNAP: return checkBinary("snap");
        case PackageManager::FLATPAK: return checkBinary("flatpak");
        default: return false;
    }
}

bool PMDetector::checkBinary(const std::string& name) {
#ifdef _WIN32
    std::string command = "where " + name + " >nul 2>&1";
    return system(command.c_str()) == 0;
#else
    std::string command = "which " + name + " >/dev/null 2>&1";
    return system(command.c_str()) == 0;
#endif
}

std::string PMDetector::getBinaryPath(const std::string& name) {
#ifdef _WIN32
    std::string command = "where " + name;
#else
    std::string command = "which " + name;
#endif
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";
    
    char buffer[256];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        // Remove trailing newline
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    }
    pclose(pipe);
    
    return result;
}

std::string PMDetector::getVersion(PackageManager pm, const std::string& path) {
    (void)path;  // Parameter reserved for future use
    std::string command;
    
    switch (pm) {
        case PackageManager::APT:
            command = "apt --version 2>&1";
            break;
        case PackageManager::PACMAN:
            command = "pacman --version 2>&1";
            break;
        case PackageManager::BREW:
            command = "brew --version 2>&1";
            break;
        case PackageManager::DNF:
            command = "dnf --version 2>&1";
            break;
        case PackageManager::YUM:
            command = "yum --version 2>&1";
            break;
        case PackageManager::WINGET:
            command = "winget --version 2>&1";
            break;
        case PackageManager::CHOCOLATEY:
            command = "choco --version 2>&1";
            break;
        default:
            return "";
    }
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";
    
    char buffer[256];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    }
    pclose(pipe);
    
    return result;
}

PMInfo PMDetector::detectAPT() {
    PMInfo info;
    if (!checkBinary("apt")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::APT;
    info.name = "apt";
    info.path = getBinaryPath("apt");
    info.version = getVersion(PackageManager::APT, info.path);
    return info;
}

PMInfo PMDetector::detectPacman() {
    PMInfo info;
    if (!checkBinary("pacman")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::PACMAN;
    info.name = "pacman";
    info.path = getBinaryPath("pacman");
    info.version = getVersion(PackageManager::PACMAN, info.path);
    return info;
}

PMInfo PMDetector::detectBrew() {
    PMInfo info;
    if (!checkBinary("brew")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::BREW;
    info.name = "brew";
    info.path = getBinaryPath("brew");
    info.version = getVersion(PackageManager::BREW, info.path);
    return info;
}

PMInfo PMDetector::detectDNF() {
    PMInfo info;
    if (!checkBinary("dnf")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::DNF;
    info.name = "dnf";
    info.path = getBinaryPath("dnf");
    info.version = getVersion(PackageManager::DNF, info.path);
    return info;
}

PMInfo PMDetector::detectYUM() {
    PMInfo info;
    if (!checkBinary("yum")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::YUM;
    info.name = "yum";
    info.path = getBinaryPath("yum");
    info.version = getVersion(PackageManager::YUM, info.path);
    return info;
}

PMInfo PMDetector::detectWinget() {
    PMInfo info;
    if (!checkBinary("winget")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::WINGET;
    info.name = "winget";
    info.path = getBinaryPath("winget");
    info.version = getVersion(PackageManager::WINGET, info.path);
    return info;
}

PMInfo PMDetector::detectChocolatey() {
    PMInfo info;
    if (!checkBinary("choco")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::CHOCOLATEY;
    info.name = "choco";
    info.path = getBinaryPath("choco");
    info.version = getVersion(PackageManager::CHOCOLATEY, info.path);
    return info;
}

PMInfo PMDetector::detectSnap() {
    PMInfo info;
    if (!checkBinary("snap")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::SNAP;
    info.name = "snap";
    info.path = getBinaryPath("snap");
    return info;
}

PMInfo PMDetector::detectFlatpak() {
    PMInfo info;
    if (!checkBinary("flatpak")) {
        info.type = PackageManager::UNKNOWN;
        return info;
    }
    
    info.type = PackageManager::FLATPAK;
    info.name = "flatpak";
    info.path = getBinaryPath("flatpak");
    return info;
}

} // namespace unipm
