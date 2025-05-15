#include "EventLogger.h"

std::ofstream EventLogger::logFile; 

void EventLogger::writeLogEvent(const std::string& message) {
    if (!logFile.is_open()) {
        openLogFile();
    } 
    
    std::time_t now = std::time(nullptr);
    char timeBuffer[100];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    logFile << "[" << timeBuffer << "] " << message << std::endl;
    
}

void EventLogger::openLogFile() {
    logFile.open("ransomware_log.txt", std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log file." << std::endl;
    }
}

void EventLogger::closeLogFile() {
    if (logFile.is_open()) {
        logFile.close();
    }
}


