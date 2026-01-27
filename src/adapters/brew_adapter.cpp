#include "unipm/adapter.h"
#include <sstream>

namespace unipm {

// Homebrew Adapter Implementation
std::string BrewAdapter::getInstallCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "brew install";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string BrewAdapter::getRemoveCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "brew uninstall";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string BrewAdapter::getUpdateCommand() {
    return "brew update && brew upgrade";
}

std::string BrewAdapter::getSearchCommand(const std::string& query) {
    return "brew search " + query;
}

std::string BrewAdapter::getListCommand() {
    return "brew list";
}

std::string BrewAdapter::getInfoCommand(const std::string& package) {
    return "brew info " + package;
}

} // namespace unipm
