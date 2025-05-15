#include "RansomGuardX.h"

RansomGuardX::RansomGuardX(const std::string& monitoredPath) {
    if (monitoredPath.empty()) {
        this->monitoredPath = SystemActions::getRecentFilesPath();
    } else {
        this->monitoredPath = monitoredPath;
    }

}

void RansomGuardX::addStrategy(std::shared_ptr<DetectionStrategy> strategy) {
    strategy->setFolderPath(monitoredPath);
    strategies.push_back(strategy);
}

void RansomGuardX::runAllScans() {
    detectionSuccess = true;

    for (const auto& strategy : strategies) {
        strategy->detectThreat();
        if(!strategy->hasRansomwareActivity) {
            detectionSuccess = false;
        }
    }

    if (detectionSuccess) {
        performSystemActions();
    }
}

void RansomGuardX::performSystemActions() {
    EventLogger::writeLogEvent("Performing system actions now...");
    hasFinishedScan = true;
    //SystemActions::disableNetworkCard();
    //SystemActions::shutDownSystem();
}