#ifndef IPROCESSMANAGER_H
#define IPROCESSMANAGER_H

#include <string>

class IProcessManager {
public:
    virtual ~IProcessManager() = default; // Virtual destructor for proper cleanup

    // Pure virtual functions for process management
    virtual void listProcesses() = 0;
    virtual void stopProcess(const std::string &pid) = 0;
    virtual void contProcess(const std::string &pid) = 0;
    virtual void killProcess(const std::string &pid) = 0;
    virtual void restartProcess(const std::string &pid) = 0;
    virtual void addProcess(const std::string &pid) = 0;
    virtual void monitorProcesses() = 0;
};

#endif // IPROCESSMANAGER_H
