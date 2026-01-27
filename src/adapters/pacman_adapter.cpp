#include "unipm/adapter.h"
#include <sstream>

namespace unipm {

// Pacman Adapter Implementation
std::string PacmanAdapter::getInstallCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "pacman -S --noconfirm";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string PacmanAdapter::getRemoveCommand(const std::vector<std::string>& packages) {
    std::ostringstream oss;
    oss << "pacman -R --noconfirm";
    for (const auto& pkg : packages) {
        oss << " " << pkg;
    }
    return oss.str();
}

std::string PacmanAdapter::getUpdateCommand() {
    return "pacman -Syu --noconfirm";
}

std::string PacmanAdapter::getSearchCommand(const std::string& query) {
    return "pacman -Ss " + query;
}

std::string PacmanAdapter::getListCommand() {
    return "pacman -Q";
}

std::string PacmanAdapter::getInfoCommand(const std::string& package) {
    return "pacman -Si " + package;
}

} // namespace unipm
