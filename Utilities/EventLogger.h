#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

class EventLogger {
    public:
        static void writeLogEvent(const std::string& message);
        static void openLogFile();
        static void closeLogFile();

    private:
        static std::ofstream logFile;  
};

#endif // EVENT_LOGGER_H