#ifndef SYSTEM_ACTIONS_H
#define SYSTEM_ACTIONS_H

#include <string>

class SystemActions {
public:
    static void shutDownSystem();
    static void disableNetworkCard();
    static std::string getRecentFilesPath();
};

#endif // SYSTEM_ACTIONS_H
