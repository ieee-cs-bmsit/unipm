#include "unipm/adapter.h"

#include <sstream>

namespace unipm {

// APT Adapter Implementation
std::string APTAdapter::getInstallCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "apt install -y";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string APTAdapter::getRemoveCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "apt remove -y";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string APTAdapter::getUpdateCommand() {
    return "apt update && apt upgrade -y";
}

std::string APTAdapter::getSearchCommand(const std::string& query) {
    return "apt search " + query;
}

std::string APTAdapter::getListCommand() {
    return "apt list --installed";
}

std::string APTAdapter::getInfoCommand(const std::string& package) {
    return "apt show " + package;
}

} // namespace unipm
