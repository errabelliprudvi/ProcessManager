#ifndef EXTENSIONHANDLER_H
#define EXTENSIONHANDLER_H

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "../../interfaces/IExtensionHandler.h"
#include "../../interfaces/ICommandExecutor.h"
#include "../../interfaces/IProcessManager.h"

using namespace std;

class ExtensionHandler : public IExtensionHandler {
public:
    void handleExtension(const string &extension, const string &fileName, const string &name, ICommandExecutor &executor, IProcessManager &processManager) override;

    bool isInstalled(const string &program);
    bool isAbsolute(string f);
};

#endif // EXTENSIONHANDLER_H
