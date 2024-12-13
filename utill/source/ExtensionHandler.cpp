#include "../header/ExtensionHandler.h"

void ExtensionHandler::handleExtension(const string &extension, const string &fileName, const string &name, ICommandExecutor &executor, IProcessManager &processManager) {
    if (extension == ".js") {
        if (isInstalled("node")) {
            cout << "Installed" << endl;
            vector<string> args = {"node", fileName + extension};
            pid_t pid = executor.execute("node", args, name);
            processManager.addProcess(to_string(pid));
        }
    }
    else if (extension == ".class") {
        if (isInstalled("java")) {
            vector<string> args;
            string fileNa = fileName;
            if (isAbsolute(fileName)) {
                size_t found = fileNa.find(name);
                if (found != std::string::npos) {
                    fileNa.erase(found);
                    args = {"java", "-cp", fileNa, name};
                }
            }
            else {
                args = {"java", fileName};
            }
            pid_t pid = executor.execute("java", args, name);
            processManager.addProcess(to_string(pid));
        }
    }
    else if (extension == ".py") {
        if (isInstalled("python3")) {
            vector<string> args = {"python3", fileName + extension};
            pid_t pid = executor.execute("python3", args, name);
            processManager.addProcess(to_string(pid));
        }
    }
    else if (extension == "") {
        string cmd = isAbsolute(fileName) ? fileName : "./" + fileName;
        vector<string> args = {fileName + extension};
        pid_t pid = executor.execute(cmd, args, name);
        processManager.addProcess(to_string(pid));
    }
    // Add more extensions if needed
}

bool ExtensionHandler::isInstalled(const string &program) {
    string command = "which " + program + " > /dev/null 2>&1";
    return (system(command.c_str()) == 0);
}

bool ExtensionHandler::isAbsolute(string f) {
    filesystem::path fileloc(f);
    return fileloc.is_absolute();
}
