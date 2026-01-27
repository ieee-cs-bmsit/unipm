#include "unipm/os_detector.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/utsname.h>
#endif

namespace unipm {

OSInfo OSDetector::detect() {
#ifdef _WIN32
    return detectWindows();
#elif __APPLE__
    return detectMacOS();
#else
    return detectLinux();
#endif
}

OSInfo OSDetector::detectLinux() {
    OSInfo info;
    info.type = OSType::LINUX;
    info.distro = LinuxDistro::UNKNOWN;
    
    // Try /etc/os-release first (standard)
    info = parseOSRelease();
    if (info.distro != LinuxDistro::UNKNOWN) {
        return info;
    }
    
    // Fallback to lsb_release
    info = detectFromLSBRelease();
    if (info.distro != LinuxDistro::UNKNOWN) {
        return info;
    }
    
    // Fallback to uname
    return detectFromUname();
}

OSInfo OSDetector::detectMacOS() {
    OSInfo info;
    info.type = OSType::MACOS;
    info.distro = LinuxDistro::UNKNOWN;
    
#ifdef __APPLE__
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        info.version = buffer.release;
    }
#endif
    
    return info;
}

OSInfo OSDetector::detectWindows() {
    OSInfo info;
    info.type = OSType::WINDOWS;
    info.distro = LinuxDistro::UNKNOWN;
    
#ifdef _WIN32
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    
    // Note: GetVersionEx is deprecated but still works
    // In production, use VerifyVersionInfo or RtlGetVersion
    #pragma warning(push)
    #pragma warning(disable: 4996)
    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        std::ostringstream oss;
        oss << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
        info.version = oss.str();
    }
    #pragma warning(pop)
#endif
    
    return info;
}

OSInfo OSDetector::parseOSRelease() {
    OSInfo info;
    info.type = OSType::LINUX;
    info.distro = LinuxDistro::UNKNOWN;
    
    std::ifstream file("/etc/os-release");
    if (!file.is_open()) {
        return info;
    }
    
    std::string line;
    std::string id, versionId, versionCodename;
    
    while (std::getline(file, line)) {
        if (line.find("ID=") == 0) {
            id = line.substr(3);
            // Remove quotes if present
            id.erase(remove(id.begin(), id.end(), '"'), id.end());
        } else if (line.find("VERSION_ID=") == 0) {
            versionId = line.substr(11);
            versionId.erase(remove(versionId.begin(), versionId.end(), '"'), versionId.end());
        } else if (line.find("VERSION_CODENAME=") == 0) {
            versionCodename = line.substr(17);
            versionCodename.erase(remove(versionCodename.begin(), versionCodename.end(), '"'), versionCodename.end());
        }
    }
    
    file.close();
    
    // Map ID to LinuxDistro enum
    if (id == "ubuntu") {
        info.distro = LinuxDistro::UBUNTU;
    } else if (id == "debian") {
        info.distro = LinuxDistro::DEBIAN;
    } else if (id == "arch") {
        info.distro = LinuxDistro::ARCH;
    } else if (id == "fedora") {
        info.distro = LinuxDistro::FEDORA;
    } else if (id == "rhel") {
        info.distro = LinuxDistro::RHEL;
    } else if (id == "centos") {
        info.distro = LinuxDistro::CENTOS;
    } else if (id == "opensuse" || id == "opensuse-leap" || id == "opensuse-tumbleweed") {
        info.distro = LinuxDistro::OPENSUSE;
    } else if (id == "gentoo") {
        info.distro = LinuxDistro::GENTOO;
    }
    
    info.version = versionId;
    info.codename = versionCodename;
    
    return info;
}

OSInfo OSDetector::detectFromLSBRelease() {
    OSInfo info;
    info.type = OSType::LINUX;
    info.distro = LinuxDistro::UNKNOWN;
    
    // Try to execute lsb_release -a
    FILE* pipe = popen("lsb_release -is 2>/dev/null", "r");
    if (!pipe) {
        return info;
    }
    
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    
    // Trim whitespace
    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    
    // Convert to lowercase for comparison
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    if (result.find("ubuntu") != std::string::npos) {
        info.distro = LinuxDistro::UBUNTU;
    } else if (result.find("debian") != std::string::npos) {
        info.distro = LinuxDistro::DEBIAN;
    } else if (result.find("arch") != std::string::npos) {
        info.distro = LinuxDistro::ARCH;
    } else if (result.find("fedora") != std::string::npos) {
        info.distro = LinuxDistro::FEDORA;
    }
    
    return info;
}

OSInfo OSDetector::detectFromUname() {
    OSInfo info;
    info.type = OSType::LINUX;
    info.distro = LinuxDistro::UNKNOWN;
    
#ifndef _WIN32
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        info.version = buffer.release;
    }
#endif
    
    return info;
}

} // namespace unipm
