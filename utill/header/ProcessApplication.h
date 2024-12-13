#ifndef PROCESSAPPLICATION_H
#define PROCESSAPPLICATION_H

#include <iostream>
#include <string>
#include "../../interfaces/ICommandExecutor.h"
#include "../../interfaces/IFileHandler.h"
#include "../../interfaces/IProcessManager.h"
#include "../../interfaces/IExtensionHandler.h"

using namespace std;

class ProcessApplication {
private:
    ICommandExecutor &executor;
    IFileHandler &fileHandler;
    IProcessManager &processManager;
    IExtensionHandler &extensionHandler;

public:
    ProcessApplication(ICommandExecutor &exec, IFileHandler &file, IProcessManager &process, IExtensionHandler &ext);

    void run(int args, char* arg[]);
};

#endif // PROCESSAPPLICATION_H
