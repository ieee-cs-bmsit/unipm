#include "unipm/resolver.h"
#include <algorithm>
#include <vector>

namespace unipm {

Resolver::Resolver(std::shared_ptr<Config> config) : config_(config) {}

ResolvedPackage Resolver::resolve(const std::string& packageName,
                                   PackageManager pm,
                                   const std::string& version) {
    ResolvedPackage result;
    result.originalName = packageName;
    result.packageManager = pm;
    result.version = version;
    result.confidence = 0.0f;
    
    // Check if package exists exactly
    if (config_->hasPackage(packageName)) {
        if (!version.empty()) {
            // Try to get version-specific mapping
            PackageInfo info = config_->getPackageInfo(packageName);
            auto versionIt = info.versionMappings.find(version);
            if (versionIt != info.versionMappings.end()) {
                auto pmIt = versionIt->second.find(pm);
                if (pmIt != versionIt->second.end()) {
                    result.resolvedName = pmIt->second;
                    result.confidence = 1.0f;
                    return result;
                }
            }
        }
        
        // Get standard mapping
        result.resolvedName = config_->getMapping(packageName, pm);
        result.confidence = 1.0f;
        return result;
    }
    
    // Fuzzy match to find suggestions
    auto suggestions = getSuggestions(packageName, 5);
    result.suggestions = suggestions;
    
    if (!suggestions.empty()) {
        // Use best match
        std::string bestMatch = suggestions[0];
        result.resolvedName = config_->getMapping(bestMatch, pm);
        result.confidence = fuzzyMatch(packageName, bestMatch);
    } else {
        // No match found, use as-is
        result.resolvedName = packageName;
        result.confidence = 0.0f;
    }
    
    return result;
}

std::vector<std::string> Resolver::getSuggestions(const std::string& packageName, size_t maxResults) {
    std::vector<std::pair<std::string, float>> scored;
    
    std::string normalizedInput = normalize(packageName);
    
    // Score all packages
    for (const std::string& pkg : config_->getAllPackageNames()) {
        float score = fuzzyMatch(normalizedInput, normalize(pkg));
        if (score > 0.3f) {  // Threshold for relevance
            scored.push_back({pkg, score});
        }
        
        // Also check aliases
        PackageInfo info = config_->getPackageInfo(pkg);
        for (const std::string& alias : info.aliases) {
            float aliasScore = fuzzyMatch(normalizedInput, normalize(alias));
            if (aliasScore > 0.3f) {
                scored.push_back({pkg, aliasScore});
            }
        }
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top results
    std::vector<std::string> results;
    size_t count = std::min(maxResults, scored.size());
    for (size_t i = 0; i < count; ++i) {
        results.push_back(scored[i].first);
    }
    
    return results;
}

float Resolver::fuzzyMatch(const std::string& a, const std::string& b) {
    if (a == b) return 1.0f;
    
    int distance = levenshteinDistance(a, b);
    int maxLen = std::max(a.length(), b.length());
    
    if (maxLen == 0) return 1.0f;
    
    return 1.0f - (static_cast<float>(distance) / maxLen);
}

int Resolver::levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    
    if (m == 0) return n;
    if (n == 0) return m;
    
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,      // deletion
                dp[i][j - 1] + 1,      // insertion
                dp[i - 1][j - 1] + cost // substitution
            });
        }
    }
    
    return dp[m][n];
}

std::string Resolver::normalize(const std::string& name) {
    std::string result = name;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    // Remove common suffixes/prefixes
    const std::vector<std::string> toRemove = {"-ce", "-desktop", "-bin", "lib"};
    for (const auto& suffix : toRemove) {
        size_t pos = result.find(suffix);
        if (pos != std::string::npos) {
            result.erase(pos, suffix.length());
        }
    }
    
    return result;
}

} // namespace unipm
