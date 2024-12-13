#ifndef IFILEHANDLER_H
#define IFILEHANDLER_H

#include <string>
#include <vector>
#include "Process.h" // Assuming Process is a class declared in Process.h

class IFileHandler {
public:
    virtual ~IFileHandler() = default; // Virtual destructor for proper cleanup

    // Pure virtual functions
    virtual bool fileExists(const std::string &fileName) = 0;
    virtual int saveProcess(const std::string &pid) = 0;
    virtual std::vector<Process> readProcesses() = 0;
    virtual int overrideProcessFile(std::vector<Process> &plist) = 0;
    virtual std::string getExtension() = 0;
    virtual std::string getName() = 0;
};

#endif // IFILEHANDLER_H
