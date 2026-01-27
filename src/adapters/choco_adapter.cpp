#include "unipm/adapter.h"
#include <sstream>

namespace unipm {

// Chocolatey Adapter Implementation
std::string ChocolateyAdapter::getInstallCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "choco install -y";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string ChocolateyAdapter::getRemoveCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "choco uninstall -y";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string ChocolateyAdapter::getUpdateCommand() {
    return "choco upgrade all -y";
}

std::string ChocolateyAdapter::getSearchCommand(const std::string& query) {
    return "choco search " + query;
}

std::string ChocolateyAdapter::getListCommand() {
    return "choco list --local-only";
}

std::string ChocolateyAdapter::getInfoCommand(const std::string& package) {
    return "choco info " + package;
}

// Adapter Factory Implementation
std::unique_ptr<PackageManagerAdapter> AdapterFactory::create(PackageManager pm) {
    switch (pm) {
        case PackageManager::APT:
            return std::make_unique<APTAdapter>();
        case PackageManager::PACMAN:
            return std::make_unique<PacmanAdapter>();
        case PackageManager::BREW:
            return std::make_unique<BrewAdapter>();
        case PackageManager::DNF:
            return std::make_unique<DNFAdapter>();
        case PackageManager::WINGET:
            return std::make_unique<WingetAdapter>();
        case PackageManager::CHOCOLATEY:
            return std::make_unique<ChocolateyAdapter>();
        default:
            return nullptr;
    }
}

} // namespace unipm
