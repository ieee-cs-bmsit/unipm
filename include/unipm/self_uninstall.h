#pragma once

#include <string>

namespace unipm {

/**
 * SelfUninstaller - Clean removal of unipm from the system
 * 
 * Removes:
 * - Binary executable
 * - Configuration directory
 * - Cache directory
 * - PATH entries from shell configs
 */
class SelfUninstaller {
public:
    /**
     * Uninstall unipm from the system
     * @param force Skip confirmation prompts
     * @return 0 on success, non-zero on failure
     */
    static int uninstall(bool force = false);
    
private:
    static bool removeBinary();
    static bool removeConfigDirectory();
    static bool removeCacheDirectory();
    static bool removeFromPath();
    
    static std::string getBinaryPath();
    static std::string getConfigDirectory();
    static std::string getCacheDirectory();
    
    static void printStep(const std::string& step, bool success);
};

} // namespace unipm
