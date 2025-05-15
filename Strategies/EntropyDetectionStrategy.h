#ifndef ENTROPY_DETECTION_STRATEGY_H
#define ENTROPY_DETECTION_STRATEGY_H

#include "DetectionStrategy.h"
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>

class EntropyDetectionStrategy : public DetectionStrategy {
public:
    EntropyDetectionStrategy(double initialEntropyThreshold = 7.0, size_t maxFileSize = 10 * 1024 * 1024); 
    bool detectThreat() override;
    void resetDetection(const std::map<std::string, std::string>& params) override;

private:
    double initialEntropyThreshold;
    double dynamicEntropyThreshold;
    size_t maxFileSize; 
    std::unordered_set<std::string> whitelistExtensions; 
    std::unordered_map<std::string, double> entropyCache; 
    std::unordered_map<std::string, std::filesystem::file_time_type> fileTimes; 
    std::chrono::steady_clock::time_point lastBaselineUpdate; 
    std::vector<double> entropyHistory; 
    std::unordered_set<std::string> highEntropyLogged;

    double calculateFileEntropy(const std::filesystem::path& filePath, bool sampleOnly = false);
    bool isWhitelistedExtension(const std::string& filePath);
    void updateDynamicThreshold();
    void initializeWhitelist();
};

#endif // ENTROPY_DETECTION_STRATEGY_H