#include <iostream>
#include "utill/header/ProcessApplication.h"
#include "utill/header/CommandExecutor.h"
#include "utill/header/FileHandler.h"
#include "utill/header/ProcessManager.h"
#include "utill/header/ExtensionHandler.h"

using namespace std;

// Main function
int main(int argc, char* argv[]) {
    // Initialize the components
    CommandExecutor executor;
    FileHandler fileHandler;
    ProcessManager processManager(fileHandler);
    ExtensionHandler extensionHandler;

    // Create the ProcessApplication object and pass all the dependencies
    ProcessApplication app(executor, fileHandler, processManager, extensionHandler);

    // Run the application with the arguments passed to the main function
    app.run(argc, argv);

    return 0;
}

