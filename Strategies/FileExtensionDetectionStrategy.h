#ifndef FILE_EXTENSION_DETECTION_STRATEGY_H
#define FILE_EXTENSION_DETECTION_STRATEGY_H

#include "DetectionStrategy.h"

#include <unordered_set>
#include <filesystem>
#include <algorithm>

class FileExtensionDetectionStrategy : public DetectionStrategy {
    public:
        FileExtensionDetectionStrategy(const std::string& extensionsFilePath);
        bool detectThreat() override;
        void resetDetection(const std::map<std::string, std::string>& params) override;

    private:
        std::unordered_set<std::string> suspiciousExtensions;
        std::unordered_set<std::string> suspiciousFileNames;
        void loadSuspiciousExtensions(const std::string& extensionsFilePath);
        bool hasSuspiciousExtension(const std::string& filePath);
        bool hasDoubleExtension(const std::string& filePath);
};

#endif // FILE_EXTENSION_DETECTION_STRATEGY_H