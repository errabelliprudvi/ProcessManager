#ifndef IEXTENSIONHANDLER_H
#define IEXTENSIONHANDLER_H

#include <string>
#include "ICommandExecutor.h"
#include "IProcessManager.h"

class IExtensionHandler {
public:
    virtual ~IExtensionHandler() = default; // Virtual destructor for proper cleanup

    // Pure virtual function for handling extensions
    virtual void handleExtension(const std::string &extension, const std::string &fileName, const std::string &name, 
                                 ICommandExecutor &executor, IProcessManager &processManager) = 0;
};

#endif // IEXTENSIONHANDLER_H
