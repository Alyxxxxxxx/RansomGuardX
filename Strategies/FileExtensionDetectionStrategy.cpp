#include "FileExtensionDetectionStrategy.h"

FileExtensionDetectionStrategy::FileExtensionDetectionStrategy(const std::string& extensionsFilePath) {
    loadSuspiciousExtensions(extensionsFilePath);
}

void FileExtensionDetectionStrategy::resetDetection(const std::map<std::string, std::string>& params) {
    hasRansomwareActivity = false;
    suspiciousFileNames.clear();
    suspiciousExtensions.clear();

    auto it = params.find("extensionsFilePath");
    if (it != params.end()) {
        loadSuspiciousExtensions(it->second);
    }
}

void FileExtensionDetectionStrategy::loadSuspiciousExtensions(const std::string& extensionsFilePath) {
    std::ifstream file(extensionsFilePath);
    std::string extension;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file '" << extensionsFilePath << "'\n";
    }

     while (std::getline(file, extension)) {
        if (!extension.empty() && extension[0] == '.') { 
            suspiciousExtensions.insert(extension);
        }
    }

    file.close();
}

bool FileExtensionDetectionStrategy::hasSuspiciousExtension(const std::string& filePath) {
    std::string extension = std::filesystem::path(filePath).extension().string();
    return suspiciousExtensions.find(extension) != suspiciousExtensions.end();
}

bool FileExtensionDetectionStrategy::hasDoubleExtension(const std::string& filePath) {
    size_t count = std::count(filePath.begin(), filePath.end(), '.');

    if (count > 1) {
        return true;
    }

    return false;
}

bool FileExtensionDetectionStrategy::detectThreat() {
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(monitoredPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            
            if ((hasSuspiciousExtension(filePath) || hasDoubleExtension(filePath)) && suspiciousFileNames.count(filePath) == 0) {
                suspiciousFileNames.insert(filePath);
                recordEvent("Suspicious file detected: " + filePath);
            }
        }
    }

    if (!suspiciousFileNames.empty()) {
        hasRansomwareActivity = true;
        return hasRansomwareActivity;
    }

    return false;
}

