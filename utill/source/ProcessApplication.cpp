#include "../header/ProcessApplication.h"
#include "../header/ColorCodes.h"

ProcessApplication::ProcessApplication(ICommandExecutor &exec, IFileHandler &file, IProcessManager &process, IExtensionHandler &ext)
    : executor(exec), fileHandler(file), processManager(process), extensionHandler(ext) {}

void ProcessApplication::run(int args, char* arg[]) {
    if (args < 3) {
        cerr << RED "Error: Insufficient arguments provided." RESET << endl;
        cerr << "Usage: ./application <command> <file>" << endl;
        return;
    }

    string cmd = arg[1];
    string fileName = arg[2];

    // Extract the file name without extension
    size_t start = fileName.find("."); // Find the position of "."
    std::string sub = fileName.substr(0, start); // Extract the name without extension
    fileName = sub;

    if (cmd == "start") {
        if (fileHandler.fileExists(fileName)) {
            string extension = fileHandler.getExtension();
            string name = fileHandler.getName();
            extensionHandler.handleExtension(extension, fileName, name, executor, processManager);
        }
        else{
            cout<<"Filenot Found"<<endl;
        }
    }
    else if (cmd == "list") {
        processManager.listProcesses();
    }
    else if (cmd == "stop") {
        processManager.stopProcess(fileName);
    }
    else if (cmd == "kill") {
        processManager.killProcess(fileName);
    }
    else if (cmd == "cont") {
        processManager.contProcess(fileName);
    }
    else if (cmd == "restart") {
        processManager.restartProcess(fileName);
    }
    else if (cmd == "monitor") {
        processManager.monitorProcesses();
    }
}
