#include "SystemActions.h"

#include <iostream>

void SystemActions::shutDownSystem() {
    std::cout << "Shutting down the system...\n";
    system("shutdown /s /t 0");
}

void SystemActions::disableNetworkCard() {
    std::cout << "Disabling network interface...\n";
    system("netsh interface set interface name=\"Wi-Fi\" admin=disable");
    system("netsh interface set interface name=\"Ethernet\" admin=disable");
}

std::string SystemActions::getRecentFilesPath() {
    char* appDataPath = nullptr;
    size_t length = 0;
    _dupenv_s(&appDataPath, &length, "APPDATA");
    std::string recentPath = std::string(appDataPath) + "\\Microsoft\\Windows\\Recent";
    free(appDataPath);
    return recentPath;
}