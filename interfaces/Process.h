// Process.h
#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
public:
    std::string processID;
    std::string name;
    std::string cmd;
    std::string filePath;

    Process(const std::string &id, const std::string &nam, const std::string &cm, const std::string &fp)
        : processID(id), name(nam), cmd(cm), filePath(fp) {}
};

#endif // PROCESS_H
