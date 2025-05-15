// FileModificationDetectionStrategy.cpp
#include "FileModificationDetectionStrategy.h"
#include <iostream>
#include <chrono>

FileModificationDetectionStrategy::FileModificationDetectionStrategy(int interval_seconds, int threshold_changes)
    : interval(interval_seconds), 
      threshold(threshold_changes), 
      last_check_time(std::chrono::steady_clock::now()) {
}

void FileModificationDetectionStrategy::resetDetection(const std::map<std::string, std::string>& params) {
    hasRansomwareActivity = false;
    changes_count = 0;
    file_times.clear();
    last_check_time = std::chrono::steady_clock::now();

    auto it = params.find("interval");
    if (it != params.end()) {
        try {
            interval = std::stoi(it->second);
        } catch (const std::exception& e) {
            recordEvent("Error: Invalid value for interval. " + std::string(e.what()));
        }
    }

    it = params.find("threshold");
    if (it != params.end()) {
        try {
            threshold = std::stoi(it->second);
        } catch (const std::exception& e) {
            recordEvent("Error: Invalid value for threshold. " + std::string(e.what()));
        }
    }
}

std::optional<std::filesystem::file_time_type> FileModificationDetectionStrategy::retrieveLastModifiedTime(const std::filesystem::path& file_path) {
    try {
        return std::filesystem::last_write_time(file_path);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: Could not retrieve the modification time of " << file_path << ": " << e.what() << '\n';
        return std::nullopt;
    }
}

bool FileModificationDetectionStrategy::detectThreat() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto time_elapsed = duration_cast<seconds>(now - last_check_time).count();

    if (time_elapsed >= interval) {
        changes_count = 0;
        last_check_time = now;
    }

    std::unordered_set<std::string> current_files;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(monitoredPath)) {
            if (std::filesystem::is_regular_file(entry.status())) {
                auto file_path = entry.path().string();
                current_files.insert(file_path);

                auto last_mod_time = retrieveLastModifiedTime(entry.path());
                if (!last_mod_time) {
                    continue; 
                }

                if (file_times.find(file_path) == file_times.end()) {
                    file_times[file_path] = *last_mod_time;
                } else if (*last_mod_time != file_times[file_path]) {
                    ++changes_count;
                    file_times[file_path] = *last_mod_time;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error while iterating over the directory " << monitoredPath << ": " << e.what() << '\n';
        return false;
    }

    for (auto it = file_times.begin(); it != file_times.end();) {
        if (current_files.find(it->first) == current_files.end()) {
            it = file_times.erase(it);
        } else {
            ++it;
        }
    }

    if (changes_count >= threshold) {
        auto elapsed_time = duration_cast<seconds>(now - last_check_time).count();  
        std::string message = "Alert: " + std::to_string(changes_count) +
                             " changes detected in files within the directory: '" + monitoredPath + "'. " +
                             "This happened in the last " + std::to_string(elapsed_time) + " seconds.";

        recordEvent(message); 
        hasRansomwareActivity = true;
        return hasRansomwareActivity;
    }

    return false;
}
