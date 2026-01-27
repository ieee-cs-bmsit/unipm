#include "unipm/adapter.h"
#include <sstream>

namespace unipm {

// DNF Adapter Implementation
std::string DNFAdapter::getInstallCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "dnf install -y";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string DNFAdapter::getRemoveCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "dnf remove -y";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string DNFAdapter::getUpdateCommand() {
    return "dnf upgrade -y";
}

std::string DNFAdapter::getSearchCommand(const std::string& query) {
    return "dnf search " + query;
}

std::string DNFAdapter::getListCommand() {
    return "dnf list installed";
}

std::string DNFAdapter::getInfoCommand(const std::string& package) {
    return "dnf info " + package;
}

} // namespace unipm
