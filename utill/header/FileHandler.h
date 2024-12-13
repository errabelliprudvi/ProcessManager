#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include "../../interfaces/IFileHandler.h"
#include "../../interfaces/Process.h"

namespace fs = std::filesystem;

class FileHandler : public IFileHandler {
public:
    std::string extension;
    std::string name;
    std::string cmd;
    std::string filePat;

    // Override methods from IFileHandler
    std::string getExtension() override;
    std::string getName() override;
    bool fileExists(const std::string &fileName) override;
    int saveProcess(const std::string &pid) override;  
    int overrideProcessFile(std::vector<Process> &plist) override;
    std::vector<Process> readProcesses() override;
};

#endif // FILE_HANDLER_H
