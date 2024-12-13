#include "../header/CommandExecutor.h"
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

pid_t CommandExecutor::execute(const std::string &command, const std::vector<std::string> &args, const std::string &name) {
    std::string file;
    // Process arguments and check for a period in each argument
    for (const std::string &s : args) {
        size_t start = s.find(".");
        if (start != std::string::npos) {
            std::string sub = s.substr(0, start);
            file = sub;
            std::cout << sub << std::endl;
        } else {
            std::cout << "No '.' found in argument: " << s << std::endl;
        }
    }

    // Ensure the logs directory exists
    if (mkdir("logs", 0755) == -1 && errno != EEXIST) {
        std::cerr << "Failed to create logs directory!" << std::endl;
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // Redirecting output and error
        int outFd = open(("logs/" + name + "_out.log").c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        int errFd = open(("logs/" + name + "_error.log").c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (outFd == -1 || errFd == -1) {
            std::cerr << "Failed to open log files!" << std::endl;
            exit(1);
        }

        // Redirecting standard output and error
        dup2(outFd, STDOUT_FILENO);
        dup2(errFd, STDERR_FILENO);
        close(outFd);
        close(errFd);

        // Prepare the arguments for execvp
        std::vector<char*> c_args;
        for (const auto &arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        // Execute the command
        if (execvp(command.c_str(), c_args.data()) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}
