#ifndef FILE_MODIFICATION_DETECTION_STRATEGY_H
#define FILE_MODIFICATION_DETECTION_STRATEGY_H

#include "DetectionStrategy.h"
#include <string>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <optional>

class FileModificationDetectionStrategy : public DetectionStrategy {
public:
    FileModificationDetectionStrategy(int interval_seconds, int threshold_changes);
    bool detectThreat() override;
    void resetDetection(const std::map<std::string, std::string>& params) override;

private:
    int interval; 
    int threshold; 
    int changes_count = 0; 
    std::chrono::steady_clock::time_point last_check_time; 
    std::unordered_map<std::string, std::filesystem::file_time_type> file_times; 
    std::optional<std::filesystem::file_time_type> retrieveLastModifiedTime(const std::filesystem::path& file_path);
};

#endif // FILE_MODIFICATION_DETECTION_STRATEGY_H