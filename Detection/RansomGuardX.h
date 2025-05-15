#ifndef RANSOM_GUARDX_H
#define RANSOM_GUARDX_H

#include "..\Strategies\DetectionStrategy.h"
#include "..\Utilities\SystemActions.h"
#include "..\Utilities\EventLogger.h"

#include <vector>
#include <memory>
#include <thread>  
#include <chrono>  

class RansomGuardX {
    public:
        bool hasFinishedScan = false;
        RansomGuardX(const std::string& monitoredPath);
        void addStrategy(std::shared_ptr<DetectionStrategy> strategy);
        void runAllScans();

    private:
        std::string monitoredPath;
        bool detectionSuccess;
        std::vector<std::shared_ptr<DetectionStrategy>> strategies;
        void performSystemActions();
};

#endif // RANSOM_GUARDX_H