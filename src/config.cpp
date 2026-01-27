#include "unipm/config.h"
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace unipm {

Config::Config() {
    // Initialize empty
}

bool Config::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        file >> data_;
        parsePackages();
        return true;
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadDefault() {
    std::string defaultPath = getDefaultConfigPath();
    return load(defaultPath);
}

void Config::mergeUserConfig(const std::string& userConfigPath) {
    std::ifstream file(userConfigPath);
    if (!file.is_open()) {
        return;
    }
    
    try {
        json userData;
        file >> userData;
        
        // Merge user data with default data
        if (userData.contains("packages")) {
            for (auto& [key, value] : userData["packages"].items()) {
                data_["packages"][key] = value;
            }
        }
        
        parsePackages();
    } catch (const json::exception& e) {
        std::cerr << "Error parsing user config: " << e.what() << std::endl;
    }
}

PackageInfo Config::getPackageInfo(const std::string& name) {
    auto it = packages_.find(name);
    if (it != packages_.end()) {
        return it->second;
    }
    
    // Check aliases
    for (const auto& [pkgName, pkgInfo] : packages_) {
        auto aliasIt = std::find(pkgInfo.aliases.begin(), pkgInfo.aliases.end(), name);
        if (aliasIt != pkgInfo.aliases.end()) {
            return pkgInfo;
        }
    }
    
    // Return empty package info
    return PackageInfo();
}

bool Config::hasPackage(const std::string& name) {
    if (packages_.find(name) != packages_.end()) {
        return true;
    }
    
    // Check aliases
    for (const auto& [pkgName, pkgInfo] : packages_) {
        auto aliasIt = std::find(pkgInfo.aliases.begin(), pkgInfo.aliases.end(), name);
        if (aliasIt != pkgInfo.aliases.end()) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> Config::getAllPackageNames() {
    std::vector<std::string> names;
    for (const auto& [name, info] : packages_) {
        names.push_back(name);
    }
    return names;
}

std::string Config::getMapping(const std::string& packageName, PackageManager pm) {
    PackageInfo info = getPackageInfo(packageName);
    
    auto it = info.pmMappings.find(pm);
    if (it != info.pmMappings.end()) {
        return it->second;
    }
    
    // Return the package name as-is if no mapping found
    return packageName;
}

void Config::parsePackages() {
    packages_.clear();
    
    if (!data_.contains("packages")) {
        return;
    }
    
    for (auto& [key, value] : data_["packages"].items()) {
        PackageInfo info;
        info.name = key;
        
        // Parse aliases
        if (value.contains("aliases") && value["aliases"].is_array()) {
            for (const auto& alias : value["aliases"]) {
                info.aliases.push_back(alias.get<std::string>());
            }
        }
        
        // Parse package manager mappings
        const std::vector<std::string> pmKeys = {
            "apt", "pacman", "brew", "dnf", "yum", "winget", "choco", "snap", "flatpak"
        };
        
        for (const auto& pmKey : pmKeys) {
            if (value.contains(pmKey)) {
                PackageManager pm = stringToPackageManager(pmKey);
                info.pmMappings[pm] = value[pmKey].get<std::string>();
            }
        }
        
        // Parse version mappings
        if (value.contains("versions")) {
            for (auto& [versionKey, versionValue] : value["versions"].items()) {
                std::map<PackageManager, std::string> versionPmMappings;
                
                for (const auto& pmKey : pmKeys) {
                    if (versionValue.contains(pmKey)) {
                        PackageManager pm = stringToPackageManager(pmKey);
                        versionPmMappings[pm] = versionValue[pmKey].get<std::string>();
                    }
                }
                
                info.versionMappings[versionKey] = versionPmMappings;
            }
        }
        
        packages_[key] = info;
    }
}

std::string Config::getDefaultConfigPath() {
#ifdef _WIN32
    // Windows: Program Files or installation directory
    return "C:\\Program Files\\unipm\\packages.json";
#else
    // Unix: /usr/share/unipm or /usr/local/share/unipm
    return "/usr/local/share/unipm/packages.json";
#endif
}

std::string Config::getUserConfigPath() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\unipm\\packages.json";
    }
    return "";
#else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return std::string(home) + "/.config/unipm/packages.json";
#endif
}

} // namespace unipm
