#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "../../interfaces/IProcessManager.h"
#include "../../interfaces/IFileHandler.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class ProcessManager : public IProcessManager {
private:
    std::vector<Process> processList;
    IFileHandler &fileHandler;

public:
    // Constructor
    ProcessManager(IFileHandler &file);

    // Method to get colored process status
    std::string getColoredProcessStatus(const std::string &state);

    // Add a process
    void addProcess(const std::string &pid) override;

    // Get process name from /proc/<pid>/cmdline
    std::string getProcessName(const std::string &pid);

    // Display process info
    void displayProcessInfo(const Process &p);

    // List all processes
    void listProcesses() override;

    // Stop a process
    void stopProcess(const std::string &fileName) override;

    // Continue a stopped process
    void contProcess(const std::string &fileName) override;

    // Restart a process
    void restartProcess(const std::string &fileName) override;

    // Kill a process
    void killProcess(const std::string &fileName) override;

    // Run commands in bash
    void runCmdsInBash(const std::string &cmd);

    // Monitor processes and restart them if necessary
    void monitorProcesses() override;
};

#endif // PROCESSMANAGER_H
