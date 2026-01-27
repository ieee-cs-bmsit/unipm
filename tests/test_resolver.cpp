#include "../include/unipm/resolver.h"
#include "../include/unipm/config.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace unipm;

int main() {
    std::cout << "Testing Package Resolver..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Load test config
    if (!config->load("../data/packages.json")) {
        std::cerr << "Failed to load packages.json" << std::endl;
        return 1;
    }
    
    Resolver resolver(config);
    
    // Test exact match
    auto result = resolver.resolve("docker", PackageManager::APT);
    assert(result.confidence == 1.0f);
    assert(result.resolvedName == "docker.io");
    std::cout << "  ✓ Exact match test passed (docker -> docker.io)" << std::endl;
    
    // Test fuzzy matching
    auto suggestions = resolver.getSuggestions("dokcer", 3);
    assert(!suggestions.empty());
    std::cout << "  ✓ Fuzzy matching test passed (dokcer -> " << suggestions[0] << ")" << std::endl;
    
    std::cout << "✓ Resolver test passed!" << std::endl;
    
    return 0;
}
