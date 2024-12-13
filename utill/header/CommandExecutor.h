#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include "../../interfaces/ICommandExecutor.h"
#include <string>
#include <vector>

class CommandExecutor : public ICommandExecutor {
public:
    // Constructor and destructor
    CommandExecutor() = default;
    ~CommandExecutor() = default;

    // Override the execute method
    pid_t execute(const std::string &command, const std::vector<std::string> &args, const std::string &name) override;

private:
    // Add any private member variables or methods if necessary
};

#endif // COMMANDEXECUTOR_H
