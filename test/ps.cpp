#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <iomanip>

// Interface Segregation Principle: Define minimal interfaces
class ICommand {
public:
    virtual void execute(const std::string& argument) = 0;
    virtual ~ICommand() = default;
};

// Single Responsibility Principle: Separate responsibilities into different classes
class ProcessManager {
private:
    std::vector<int> processes; // Stores process IDs

public:
    void addProcess(int pid) {
        processes.push_back(pid);
    }

    void listProcesses() {
        std::cout << std::setw(10) << "PID" << std::setw(20) << "STATUS" << std::endl;
        std::cout << std::string(30, '-') << std::endl;

        for (int pid : processes) {
            std::string status = (kill(pid, 0) == 0) ? "Running" : "Stopped";
            std::cout << std::setw(10) << pid << std::setw(20) << status << std::endl;
        }
    }

    bool processExists(int pid) {
        return std::find(processes.begin(), processes.end(), pid) != processes.end();
    }

    void terminateProcess(int pid) {
        if (kill(pid, SIGKILL) == 0) {
            std::cout << "Terminated process with PID: " << pid << std::endl;
            processes.erase(std::remove(processes.begin(), processes.end(), pid), processes.end());
        } else {
            perror("Failed to terminate process");
        }
    }
};

// Open/Closed Principle: New functionality should be added without modifying existing code
class StartProcessCommand : public ICommand {
private:
    ProcessManager& processManager;

    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

public:
    StartProcessCommand(ProcessManager& pm) : processManager(pm) {}

    void execute(const std::string& filename) override {
        if (!fileExists(filename)) {
            std::cerr << "File " << filename << " does not exist." << std::endl;
            return;
        }

        pid_t pid = fork();
        if (pid == 0) { // Child process
            char* args[] = {(char*)filename.c_str(), NULL};
            execvp(args[0], args);
            std::cerr << "Failed to execute " << filename << std::endl;
            exit(EXIT_FAILURE);
        } else if (pid > 0) { // Parent process
            processManager.addProcess(pid);
            std::cout << "Started process with PID: " << pid << std::endl;
        } else {
            perror("Fork failed");
        }
    }
};

class ListProcessesCommand : public ICommand {
private:
    ProcessManager& processManager;

public:
    ListProcessesCommand(ProcessManager& pm) : processManager(pm) {}

    void execute(const std::string& argument) override {
        processManager.listProcesses();
    }
};

class StopProcessCommand : public ICommand {
private:
    ProcessManager& processManager;

public:
    StopProcessCommand(ProcessManager& pm) : processManager(pm) {}

    void execute(const std::string& pidStr) override {
        int pid = std::stoi(pidStr);
        if (!processManager.processExists(pid)) {
            std::cerr << "Process with PID " << pid << " does not exist." << std::endl;
            return;
        }

        if (kill(pid, SIGSTOP) == 0) {
            std::cout << "Stopped process with PID: " << pid << std::endl;
        } else {
            perror("Failed to stop process");
        }
    }
};

class TerminateProcessCommand : public ICommand {
private:
    ProcessManager& processManager;

public:
    TerminateProcessCommand(ProcessManager& pm) : processManager(pm) {}

    void execute(const std::string& pidStr) override {
        int pid = std::stoi(pidStr);
        if (!processManager.processExists(pid)) {
            std::cerr << "Process with PID " << pid << " does not exist." << std::endl;
            return;
        }

        processManager.terminateProcess(pid);
    }
};

// Dependency Inversion Principle: High-level modules should not depend on low-level modules; both should depend on abstractions
class CommandExecutor {
private:
    std::map<std::string, std::shared_ptr<ICommand>> commands;

public:
    void registerCommand(const std::string& name, std::shared_ptr<ICommand> command) {
        commands[name] = command;
    }

    void executeCommand(const std::string& name, const std::string& argument) {
        if (commands.find(name) == commands.end()) {
            std::cerr << "Command " << name << " not found." << std::endl;
            return;
        }
        commands[name]->execute(argument);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./process_manager <command> <argument>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string commandName = argv[1];
    std::string argument = argv[2];

    ProcessManager processManager;
    CommandExecutor executor;

    // Register commands
    executor.registerCommand("start", std::make_shared<StartProcessCommand>(processManager));
    executor.registerCommand("list", std::make_shared<ListProcessesCommand>(processManager));
    executor.registerCommand("stop", std::make_shared<StopProcessCommand>(processManager));
    executor.registerCommand("terminate", std::make_shared<TerminateProcessCommand>(processManager));

    // Execute the requested command
    executor.executeCommand(commandName, argument);

    return 0;
}
