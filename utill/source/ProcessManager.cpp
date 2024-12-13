#include "../header/ProcessManager.h"

ProcessManager::ProcessManager(IFileHandler &file) : fileHandler(file) {}

std::string ProcessManager::getColoredProcessStatus(const std::string &state) {
    if (state == "R") return "\033[32mRunning\033[0m";  // Green
    if (state == "S") return "\033[33mSleeping\033[0m"; // Yellow
    if (state == "D") return "\033[36mWaiting\033[0m";  // Cyan
    if (state == "T") return "\033[31mStopped\033[0m";  // Red
    if (state == "Z") return "\033[36mZombie\033[0m";   // Cyan
    return "\033[37mUnknown\033[0m";                    // White
}

void ProcessManager::addProcess(const std::string &pid) {
    fileHandler.saveProcess(pid);
}

std::string ProcessManager::getProcessName(const std::string &pid) {
    std::string cmdlineFile = "/proc/" + pid + "/cmdline";
    std::ifstream cmdlineStream(cmdlineFile);
    
    if (!cmdlineStream.is_open()) {
        return "Unknown";
    }

    std::string cmdline;
    getline(cmdlineStream, cmdline, '\0');
    cmdlineStream.close();

    if (cmdline.empty()) {
        return "Unknown";
    }

    size_t lastSlash = cmdline.find_last_of('/');
    std::string processName = lastSlash != std::string::npos ? cmdline.substr(lastSlash + 1) : cmdline;

    size_t dotPosition = processName.find_last_of('.');
    if (dotPosition != std::string::npos) {
        processName = processName.substr(0, dotPosition);
    }

    return processName;
}

void ProcessManager::displayProcessInfo(const Process &p) {
    std::string statFile = "/proc/" + p.processID + "/stat";
    std::ifstream statStream(statFile);
    
    if (!statStream.is_open()) {
        std::cout << "Error opening stat file for PID " << p.processID << std::endl;
        return;
    }

    std::string line;
    getline(statStream, line);

    std::stringstream ss(line);
    std::string comm, state;
    long pidInt, ppid, vsize, rss, utime, stime, starttime;
    ss >> pidInt >> comm >> state >> ppid >> vsize >> rss >> utime >> stime >> starttime;
    
    comm = comm.substr(1, comm.size() - 2);  // Clean command name

    std::string processName = p.name;
    long totalTime = utime + stime;

    long sysUptime;
    std::ifstream uptimeFile("/proc/uptime");
    uptimeFile >> sysUptime;
    long uptime = sysUptime - starttime / sysconf(_SC_CLK_TCK);

    std::string stateColor = (state == "R" ? "\033[32m" : (state == "S" ? "\033[33m" : "\033[31m"));
    std::string processtatus = getColoredProcessStatus(state);

    std::cout << "| " 
              << std::left << std::setw(7) << p.processID << " | "
              << std::left << std::setw(40) << processName << " | "
              << std::left << std::setw(7) << processtatus << " | "
              << std::right << std::setw(20) << rss * sysconf(_SC_PAGE_SIZE) / 1024 << " KB" << " | "
              << std::right << std::setw(15) << totalTime << " ticks" << " | "
              << std::right << std::setw(8) << uptime << "s" << " |" << std::endl;
}

void ProcessManager::listProcesses() {
    processList = fileHandler.readProcesses();
    if (processList.empty()) {
        std::cout << "No active processes" << std::endl;
        return;
    }

    std::cout << "+" << std::setw(7) << "---------" << "+" << std::setw(40) << "------------------------------------------" << "+"
              << std::setw(10) << "------------" << "+" << std::setw(20) << "-------------------------" << "+"
              << std::setw(15) << "-----------------------" << "+" << std::setw(8) << "-----------" << "+" << std::endl;

    std::cout << "| " << std::setw(7) << "PID" << " | " << std::setw(40) << "NAME" << " | " << std::setw(10) << "STATE" << " | "
              << std::setw(20) << "MEMORY" << " | " << std::setw(15) << "CPU TIME" << " | " << std::setw(8) << "UPTIME" << " |" << std::endl;
    std::cout << "+" << std::setw(7) << "---------" << "+" << std::setw(40) << "------------------------------------------" << "+"
              << std::setw(10) << "------------" << "+" << std::setw(20) << "-------------------------" << "+"
              << std::setw(15) << "-----------------------" << "+" << std::setw(8) << "-----------" << "+" << std::endl;

    for (const Process &p : processList) {
        displayProcessInfo(p);
    }

    std::cout << "+" << std::setw(7) << "---------" << "+" << std::setw(40) << "------------------------------------------" << "+"
              << std::setw(10) << "------------" << "+" << std::setw(20) << "-------------------------" << "+"
              << std::setw(15) << "-----------------------" << "+" << std::setw(8) << "-----------" << "+" << std::endl;
}

void ProcessManager::stopProcess(const std::string &fileName) {
    processList = fileHandler.readProcesses();
    for (const auto &process : processList) {
        if (process.name == fileName) {
            pid_t pid = std::stoi(process.processID);
            if (kill(pid, SIGSTOP) == -1) {
                perror("Error stopping process");
            } else {
                std::cout << "Process " << fileName << " successfully stopped." << std::endl;
            }
            return;
        }
    }
    std::cout << "Process " << fileName << " not found in process list." << std::endl;
}

void ProcessManager::contProcess(const std::string &fileName) {
    processList = fileHandler.readProcesses();
    for (const auto &process : processList) {
        if (process.name == fileName) {
            pid_t pid = std::stoi(process.processID);
            if (kill(pid, SIGCONT) == -1) {
                perror("Error continuing process");
            } else {
                std::cout << "Process " << fileName << " successfully resumed." << std::endl;
            }
            return;
        }
    }
    std::cout << "Process " << fileName << " not found in process list." << std::endl;
}

void ProcessManager::restartProcess(const std::string &fileName) {
    processList = fileHandler.readProcesses();
    for (const Process p : processList) {
        if (p.name == fileName) {
            killProcess(fileName);
            runCmdsInBash("./Psm start " + p.filePath);
            return;
        }
    }
    std::cout << "Process " << fileName << " not found." << std::endl;
}

void ProcessManager::killProcess(const std::string &fileName) {
    processList = fileHandler.readProcesses();
    if (processList.empty()) {
        std::cout << "No processes found." << std::endl;
        return;
    }

    bool processFound = false;
    processList.erase(std::remove_if(processList.begin(), processList.end(),
        [&](const Process &process) {
            if (process.name == fileName) {
                processFound = true;
                pid_t pid = std::stoi(process.processID);
                if (kill(pid, SIGKILL) == -1) {
                    if (errno == ESRCH) {
                        std::cout << "Process not found: " << pid << std::endl;
                    } else {
                        perror("Error killing process");
                    }
                } else {
                    std::cout << "Process " << fileName << " with PID: " << pid << " successfully killed." << std::endl;
                }
                return true;
            }
            return false;
        }), processList.end());

    if (!processFound) {
        std::cout << "Process " << fileName << " not found." << std::endl;
    } else {
        fileHandler.overrideProcessFile(processList);
    }
}

void ProcessManager::runCmdsInBash(const std::string &cmd) {
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cout << "Command failed!" << std::endl;
    }
}

void ProcessManager::monitorProcesses() {
    int s = 1;
    while (s > 0) {
        processList = fileHandler.readProcesses();
        s = processList.size();
        for (Process &process : processList) {
            int status;
            pid_t result = waitpid(std::stoi(process.processID), &status, WNOHANG);  // Non-blocking

            if (result == -1) {
                std::cerr << "Error with waitpid for process " << process.name << std::endl;
            } else if (result == 0) {
                continue;  // Process is still running
            } else {
                if (WIFEXITED(status)) {
                    std::cout << "Process " << process.name << " (PID: " << process.processID << ") exited normally with status " << WEXITSTATUS(status) << std::endl;
                } else if (WIFSIGNALED(status)) {
                    std::cout << "Process " << process.name << " (PID: " << process.processID << ") terminated by signal " << WTERMSIG(status) << std::endl;
                }

                std::cout << "Process " << process.name << " is not running. Restarting..." << std::endl;
                std::cout << "Process " << process.name << " restarted with PID: " << process.processID << std::endl;
            }
        }
        sleep(5);  // Check every 5 seconds
    }
}
