#include "unipm/adapter.h"
#include <sstream>

namespace unipm {

// Winget Adapter Implementation
std::string WingetAdapter::getInstallCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "winget install";
    for (const auto& pkg : packages) {
        oss << " --id " << pkg << " --silent --accept-package-agreements --accept-source-agreements";
    }
    return oss.str();
}

std::string WingetAdapter::getRemoveCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "winget uninstall";
    for (const auto& pkg : packages) {
        oss << " --id " << pkg << " --silent";
    }
    return oss.str();
}

std::string WingetAdapter::getUpdateCommand() {
    return "winget upgrade --all --silent --accept-package-agreements --accept-source-agreements";
}

std::string WingetAdapter::getSearchCommand(const std::string& query) {
    return "winget search " + query;
}

std::string WingetAdapter::getListCommand() {
    return "winget list";
}

std::string WingetAdapter::getInfoCommand(const std::string& package) {
    return "winget show --id " + package;
}

} // namespace unipm
