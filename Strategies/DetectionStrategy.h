#ifndef DETECTION_STRATEGY_H
#define DETECTION_STRATEGY_H

#include "..\Utilities\EventLogger.h"

#include <string>
#include <map>

class DetectionStrategy {
    public:
        bool hasRansomwareActivity = false;
        virtual bool detectThreat() = 0;
        virtual void resetDetection(const std::map<std::string, std::string>& params) = 0;

        void recordEvent(const std::string& event) {
            EventLogger::writeLogEvent(event);
        };

        void setFolderPath(const std::string& path) {
            monitoredPath = path;
        };
    
    protected:
        std::string monitoredPath;
        
};

#endif // DETECTION_STRATEGY_H