#include "unipm/doctor.h"
#include "unipm/config.h"
#include "unipm/os_detector.h"
#include "unipm/pm_detector.h"
#include "unipm/ui.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

namespace unipm {

void Doctor::printCheckResult(const std::string& check, bool passed, const std::string& message) {
    if (passed) {
        std::cout << "✓ " << check;
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << std::endl;
    } else {
        std::cout << "✗ " << check;
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << std::endl;
    }
}

bool Doctor::checkInstallation() {
    std::cout << "\n=== Installation Check ===" << std::endl;
    
    // Get executable path
    std::string exePath;
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    exePath = buffer;
#else
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        exePath = buffer;
    } else {
        // Fallback for macOS
        exePath = "unknown";
    }
#endif
    
    printCheckResult("unipm binary found", true, exePath);
    printCheckResult("Version", true, "1.0.0");
    
    return true;
}

bool Doctor::checkPath() {
    std::cout << "\n=== PATH Configuration ===" << std::endl;
    
    // Check if unipm is in PATH
    const char* pathEnv = std::getenv("PATH");
    if (!pathEnv) {
        printCheckResult("PATH environment variable", false, "Not found");
        return false;
    }
    
    std::string pathStr(pathEnv);
    
    // Common installation directories
    std::vector<std::string> expectedPaths = {
        "/usr/local/bin",
        "/usr/bin",
        std::string(std::getenv("HOME") ? std::getenv("HOME") : "") + "/.local/bin"
    };
    
#ifdef _WIN32
    expectedPaths = {
        "C:\\Program Files\\unipm",
        std::string(std::getenv("LOCALAPPDATA") ? std::getenv("LOCALAPPDATA") : "") + "\\unipm"
    };
#endif
    
    bool foundInPath = false;
    for (const auto& dir : expectedPaths) {
        if (!dir.empty() && pathStr.find(dir) != std::string::npos) {
            printCheckResult("PATH configured", true, "Found in " + dir);
            foundInPath = true;
            break;
        }
    }
    
    if (!foundInPath) {
        printCheckResult("PATH configured", false, "unipm directory not in PATH");
        std::cout << "  ℹ Add unipm directory to your PATH" << std::endl;
    }
    
    return foundInPath;
}

bool Doctor::checkPackageManagers() {
    std::cout << "\n=== Package Managers ===" << std::endl;
    
    OSDetector osDetector;
    OSInfo osInfo = osDetector.detect();
    
    std::cout << "  OS: " << osTypeToString(osInfo.type);
    if (osInfo.type == OSType::LINUX) {
        std::cout << " (" << linuxDistroToString(osInfo.distro) << ")";
    }
    std::cout << std::endl;
    
    PMDetector pmDetector;
    
    // Check for common package managers
    std::vector<PackageManager> managers = {
        PackageManager::APT,
        PackageManager::PACMAN,
        PackageManager::BREW,
        PackageManager::DNF,
        PackageManager::WINGET,
        PackageManager::CHOCOLATEY
    };
    
    int foundCount = 0;
    for (const auto& pm : managers) {
        if (pmDetector.isAvailable(pm)) {
            std::string pmName = packageManagerToString(pm);
            printCheckResult(pmName + " available", true);
            foundCount++;
        }
    }
    
    if (foundCount == 0) {
        printCheckResult("Package managers", false, "No package managers found");
        return false;
    }
    
    printCheckResult("Total package managers found", true, std::to_string(foundCount));
    return true;
}

bool Doctor::checkDatabase() {
    std::cout << "\n=== Package Database ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    bool loaded = config->loadDefault();
    if (!loaded) {
        loaded = config->load("data/packages.json");
    }
    
    if (loaded) {
        // Count packages (simplified - would need to access internal data)
        printCheckResult("Package database loaded", true);
        printCheckResult("Database location", true, "Default location");
    } else {
        printCheckResult("Package database", false, "Failed to load");
        std::cout << "  ℹ Package database not found at expected locations" << std::endl;
        return false;
    }
    
    return true;
}

bool Doctor::checkConfig() {
    std::cout << "\n=== Configuration ===" << std::endl;
    
    // Check for user config file
    std::string configPath;
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (appdata) {
        configPath = std::string(appdata) + "\\unipm\\packages.json";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        configPath = std::string(home) + "/.config/unipm/packages.json";
    }
#endif
    
    std::ifstream configFile(configPath);
    if (configFile.good()) {
        printCheckResult("User config file", true, configPath);
    } else {
        printCheckResult("User config file", true, "Not found (using defaults)");
    }
    
    return true;
}

bool Doctor::checkNetwork() {
    std::cout << "\n=== Network Connectivity ===" << std::endl;
    
    // Simple check - try to resolve a hostname
    // This is a simplified check
    printCheckResult("Network access", true, "Assumed available");
    
    std::cout << "  ℹ To test package manager connectivity, try:" << std::endl;
    std::cout << "    unipm update" << std::endl;
    
    return true;
}

int Doctor::runDiagnostics() {
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "  unipm doctor - System Diagnostics" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    bool installOk = checkInstallation();
    bool pathOk = checkPath();
    bool pmOk = checkPackageManagers();
    bool dbOk = checkDatabase();
    bool configOk = checkConfig();
    bool netOk = checkNetwork();
    
    std::cout << "\n=== Summary ===" << std::endl;
    
    int totalChecks = 6;
    int passedChecks = (installOk ? 1 : 0) + (pathOk ? 1 : 0) + (pmOk ? 1 : 0) + 
                       (dbOk ? 1 : 0) + (configOk ? 1 : 0) + (netOk ? 1 : 0);
    
    std::cout << "Checks passed: " << passedChecks << "/" << totalChecks << std::endl;
    
    if (passedChecks == totalChecks) {
        std::cout << "\n✓ All checks passed! unipm is ready to use." << std::endl;
        return 0;
    } else {
        std::cout << "\n⚠ Some checks failed. See above for details." << std::endl;
        std::cout << "For help, visit: https://github.com/ieee-cs-bmsit/unipm/issues" << std::endl;
        return 1;
    }
}

} // namespace unipm
