
#include "../header/FileHandler.h"




std::string FileHandler::getExtension() {
    return extension;
}

std::string FileHandler::getName() {
    return name;
}

bool FileHandler::fileExists(const std::string &fileName) {
    fs::path fileloc(fileName);
    std::string folderPath = fs::current_path()/fileloc.parent_path(); // Current directory

    // Check if the provided path is absolute
    if (fileloc.is_absolute()) {
        if (fs::exists(fileloc) && fs::is_regular_file(fileloc)) {
            extension = fileloc.extension().string();
            filePat = fileloc.string();
            name = fileloc.stem().string();
            std::cout << "File found: " << fileloc.filename() << std::endl;
            std::cout << "Extension: " << extension << std::endl;
            return true;
        }

        std::cout << "File not found. Searching without extension..." << std::endl;
        folderPath = fileloc.parent_path();
    }

    // Search for the file without extension in the folder
    for (const auto &entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string fullFileName = entry.path().filename().string();
            std::string nameWithoutExt = entry.path().stem().string();
            if (nameWithoutExt == fileloc.stem().string()) {
                extension = entry.path().extension().string();
                filePat = folderPath + "/" + fullFileName;
                name = nameWithoutExt;
                std::cout << "File found: " << fullFileName << std::endl;
                std::cout << "Extension: " << extension << std::endl;
                return true;
            }
        }
    }

    return false;
}

int FileHandler::saveProcess(const std::string &pid) {
    std::ofstream outFile("processes.txt", std::ios::app); // Append mode
    if (outFile.is_open()) {
        outFile << pid << "#" << name << "#" << cmd << "#" << filePat << "#" << std::endl;
        outFile.close();
        std::cout << "Process info saved." << std::endl;
    } else {
        std::cout << "Unable to open file for writing!" << std::endl;
    }
    return 0;
}


int FileHandler::overrideProcessFile(std::vector<Process> &plist) {
    std::ofstream outFile("processes.txt"); // Override mode
    if (outFile.is_open()) {
        for (const Process &p : plist) {
            outFile << p.processID << "#" << p.name << "#" << p.cmd << "#" << p.filePath << "#" << std::endl;
        }
        outFile.close();
        std::cout << "Processes info saved." << std::endl;
    } else {
        std::cout << "Unable to open file for writing!" << std::endl;
    }
    return 0;
}

std::vector<Process> FileHandler::readProcesses() {
    std::vector<Process> processList;
    std::ifstream inFile("processes.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::stringstream ss(line);
            std::string processId, name, cmd, filePath;

            if (std::getline(ss, processId, '#') &&
                std::getline(ss, name, '#') &&
                std::getline(ss, cmd, '#') &&
                std::getline(ss, filePath, '#')) {
                processList.push_back(Process(processId, name, cmd, filePath));
                std::cout << "Process ID: " << processId << ", File Path: " << filePath << std::endl;
            }
        }
        inFile.close();
    } else {
        std::cout << "Unable to open file for reading!" << std::endl;
    }
    return processList;
}
