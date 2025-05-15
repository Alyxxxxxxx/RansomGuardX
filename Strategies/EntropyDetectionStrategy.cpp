#include "EntropyDetectionStrategy.h"
#include <fstream>
#include <cmath>
#include <array>
#include <algorithm>
#include <numeric>

EntropyDetectionStrategy::EntropyDetectionStrategy(double initialEntropyThreshold, size_t maxFileSize)
    : initialEntropyThreshold(initialEntropyThreshold),
      dynamicEntropyThreshold(initialEntropyThreshold),
      maxFileSize(maxFileSize),
      lastBaselineUpdate(std::chrono::steady_clock::now()) {
    initializeWhitelist();
}

void EntropyDetectionStrategy::resetDetection(const std::map<std::string, std::string>& params) {
    hasRansomwareActivity = false;
    entropyCache.clear();
    fileTimes.clear();
    highEntropyLogged.clear();
    entropyHistory.clear();

    auto it = params.find("initialEntropyThreshold");
    if (it != params.end()) {
        try {
            double newThreshold = std::stod(it->second);
            initialEntropyThreshold = newThreshold;
            dynamicEntropyThreshold = newThreshold;
        } catch (const std::exception& e) {
            recordEvent("Error: Invalid value for initialEntropyThreshold. " + std::string(e.what()));
        }
    }
}


void EntropyDetectionStrategy::initializeWhitelist() {
    whitelistExtensions = {
        ".zip", ".rar", ".7z", ".gz", ".bz2", 
        ".jpg", ".jpeg", ".png", ".gif", ".mp4", ".avi", ".mkv", 
         ".exe", ".dll" 
    };
}

bool EntropyDetectionStrategy::isWhitelistedExtension(const std::string& filePath) {
    std::string extension = std::filesystem::path(filePath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return whitelistExtensions.find(extension) != whitelistExtensions.end();
}

double EntropyDetectionStrategy::calculateFileEntropy(const std::filesystem::path& filePath, bool sampleOnly) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        recordEvent("Error: Could not open file for entropy calculation: " + filePath.string());
        return 0.0;
    }

    size_t bytesToRead = sampleOnly ? std::min(maxFileSize, static_cast<size_t>(1024 * 1024)) : maxFileSize; 
    std::array<unsigned long, 256> frequency = {0};
    std::vector<char> buffer(16384); 
    size_t totalBytes = 0;

    while (file && totalBytes < bytesToRead) {
        file.read(buffer.data(), buffer.size());
        size_t bytesRead = file.gcount();
        for (size_t i = 0; i < bytesRead; ++i) {
            frequency[static_cast<unsigned char>(buffer[i])]++;
            totalBytes++;
        }
    }
    file.close();

    if (totalBytes == 0) {
        return 0.0; 
    }

    double entropy = 0.0;
    for (unsigned long freq : frequency) {
        if (freq > 0) {
            double probability = static_cast<double>(freq) / totalBytes;
            entropy -= probability * std::log2(probability);
        }
    }

    return entropy;
}

void EntropyDetectionStrategy::updateDynamicThreshold() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto timeElapsed = duration_cast<seconds>(now - lastBaselineUpdate).count();

    if (timeElapsed < 60 || entropyHistory.empty()) {
        return;
    }

    lastBaselineUpdate = now;
    entropyHistory.clear();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(monitoredPath)) {
            if (std::filesystem::is_regular_file(entry.status())) {
                std::string filePath = entry.path().string();
                if (isWhitelistedExtension(filePath)) {
                    continue;
                }
                if (std::filesystem::file_size(entry.path()) > maxFileSize) {
                    continue;
                }
                double entropy = calculateFileEntropy(entry.path());
                if (entropy > 0.0) {
                    entropyHistory.push_back(entropy);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        recordEvent("Error iterating directory for baseline: " + monitoredPath + ": " + e.what());
    }

    if (!entropyHistory.empty()) {
        double avgEntropy = std::accumulate(entropyHistory.begin(), entropyHistory.end(), 0.0) / entropyHistory.size();
        dynamicEntropyThreshold = std::clamp(avgEntropy + 1.5, initialEntropyThreshold, 7.8); 
    }
}

bool EntropyDetectionStrategy::detectThreat() {
    hasRansomwareActivity = false;
    updateDynamicThreshold(); 

    try {
        for (const auto& entry : std::filesystem::directory_iterator(monitoredPath)) {
            if (!std::filesystem::is_regular_file(entry.status())) {
                continue;
            }

            std::string filePath = entry.path().string();
            if (isWhitelistedExtension(filePath)) {
                continue; 
            }

            auto lastModTime = std::filesystem::last_write_time(entry.path());
            if (fileTimes.find(filePath) != fileTimes.end() && fileTimes[filePath] == lastModTime) {
                if (entropyCache.find(filePath) != entropyCache.end() && entropyCache[filePath] > dynamicEntropyThreshold) {
                    hasRansomwareActivity = true;
                    if (highEntropyLogged.find(filePath) == highEntropyLogged.end()) {
                        recordEvent("High entropy detected in file: " + filePath + ", Entropy: " + std::to_string(entropyCache[filePath]));
                        highEntropyLogged.insert(filePath);
                    }
                }
                continue; 
            }

            bool sampleOnly = std::filesystem::file_size(entry.path()) > maxFileSize;
            if (sampleOnly && maxFileSize == 0) {
                continue; 
            }

            double entropy = calculateFileEntropy(entry.path(), sampleOnly);
            entropyCache[filePath] = entropy;
            fileTimes[filePath] = lastModTime;

            if (entropy > dynamicEntropyThreshold) {
                hasRansomwareActivity = true;
                if (highEntropyLogged.find(filePath) == highEntropyLogged.end()) {
                    recordEvent("High entropy detected in file: " + filePath + ", Entropy: " + std::to_string(entropy));
                    highEntropyLogged.insert(filePath);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        recordEvent("Error iterating directory " + monitoredPath + ": " + e.what());
        return false;
    }

    for (auto it = fileTimes.begin(); it != fileTimes.end();) {
        if (!std::filesystem::exists(it->first)) {
            entropyCache.erase(it->first);
            highEntropyLogged.erase(it->first); 
            it = fileTimes.erase(it);
        } else {
            if (std::filesystem::last_write_time(it->first) != it->second) {
                highEntropyLogged.erase(it->first);
            }
            ++it;
        }
    }

    return hasRansomwareActivity;
}