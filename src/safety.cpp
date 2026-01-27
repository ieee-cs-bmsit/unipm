#include "unipm/safety.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace unipm {

std::string Safety::sanitize(const std::string& input) {
    std::string sanitized = input;
    
    // Remove dangerous characters
    const std::string dangerous = ";|&$`<>(){}[]'\"\\";
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
        [&dangerous](char c) {
            return dangerous.find(c) != std::string::npos;
        }), sanitized.end());
    
    return sanitized;
}

bool Safety::isValidPackageName(const std::string& name) {
    if (name.empty() || name.length() > 256) {
        return false;
    }
    
    // Allow alphanumeric, dash, underscore, dot, @, and forward slash (for scoped packages)
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != '.' && c != '@' && c != '/') {
            return false;
        }
    }
    
    return true;
}

bool Safety::isValidCommand(const std::string& command) {
    if (command.empty()) {
        return false;
    }
    
    // Check for dangerous patterns
    if (containsDangerousChars(command)) {
        return false;
    }
    
    return true;
}

std::string Safety::escapeShell(const std::string& input) {
    std::string escaped;
    
    for (char c : input) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\'' || c == '"' || c == '\\') {
            escaped += '\\';
        }
        escaped += c;
    }
    
    return escaped;
}

void Safety::logOperation(const std::string& command, bool success) {
    std::string historyPath = getHistoryPath();
    
    std::ofstream file(historyPath, std::ios::app);
    if (!file.is_open()) {
        return;
    }
    
    // Get current timestamp
    time_t now = time(nullptr);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    file << "[" << timestamp << "] ";
    file << (success ? "SUCCESS" : "ATTEMPT") << ": ";
    file << command << std::endl;
    
    file.close();
}

std::string Safety::getHistoryPath() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\unipm\\history.log";
    }
    return "unipm_history.log";
#else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return std::string(home) + "/.unipm/history.log";
#endif
}

bool Safety::containsDangerousChars(const std::string& input) {
    // Check for obvious shell injection attempts
    const std::vector<std::string> dangerousPatterns = {
        "$(", "`", "&&", "||", ";", "|", ">", "<", "rm -rf"
    };
    
    for (const auto& pattern : dangerousPatterns) {
        if (input.find(pattern) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

} // namespace unipm
