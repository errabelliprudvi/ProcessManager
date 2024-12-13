#ifndef ICOMMANDEXECUTOR_H
#define ICOMMANDEXECUTOR_H

#include <string>
#include <vector>
#include <sys/types.h> // For pid_t

class ICommandExecutor {
public:
    virtual ~ICommandExecutor() = default; // Virtual destructor for proper cleanup

    // Pure virtual function for command execution
    virtual pid_t execute(const std::string &command, const std::vector<std::string> &args, const std::string &name) = 0;
};

#endif // ICOMMANDEXECUTOR_H
