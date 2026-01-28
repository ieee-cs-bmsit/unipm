#pragma once

#include <string>

namespace unipm {

/**
 * Doctor - System diagnostics and health check
 * 
 * Checks:
 * - unipm installation and version
 * - PATH configuration
 * - Available package managers
 * - Package database status
 * - Configuration file validity
 * - Network connectivity
 */
class Doctor {
public:
    /**
     * Run comprehensive system diagnostics
     * @return 0 if all checks pass, non-zero otherwise
     */
    static int runDiagnostics();
    
private:
    static bool checkInstallation();
    static bool checkPath();
    static bool checkPackageManagers();
    static bool checkDatabase();
    static bool checkConfig();
    static bool checkNetwork();
    
    static void printCheckResult(const std::string& check, bool passed, const std::string& message = "");
};

} // namespace unipm
