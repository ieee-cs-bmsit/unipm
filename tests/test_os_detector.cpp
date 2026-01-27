#include "../include/unipm/os_detector.h"
#include <iostream>
#include <cassert>

using namespace unipm;

int main() {
    std::cout << "Testing OS Detector..." << std::endl;
    
    OSDetector detector;
    OSInfo info = detector.detect();
    
    // Basic validation
    assert(info.type != OSType::UNKNOWN);
    
    std::cout << "  Detected OS: " << osTypeToString(info.type) << std::endl;
    
    if (info.type == OSType::LINUX) {
        std::cout << "  Distribution: " << linuxDistroToString(info.distro) << std::endl;
        std::cout << "  Version: " << info.version << std::endl;
    }
    
    std::cout << "✓ OS Detector test passed!" << std::endl;
    
    return 0;
}
