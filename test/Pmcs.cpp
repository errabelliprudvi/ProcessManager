#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <filesystem>
#include <cstdlib>
#include <iomanip>



#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;

namespace fs = std::filesystem;

// ANSI Escape codes for colors and styling
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


class Process
{
   
 public:
    string processID;
    string  name ;
    string cmd ;
    string filePath ;

    public:
    Process(string &id,string &nam,string &cm,string &fp):processID(id),name(nam),cmd(cm),filePath(fp){}
};

// File System Handling
class IFileHandler {

public:
    virtual bool fileExists(const string &fileName) = 0;
    virtual int saveProcess(string pid) = 0;
    virtual vector<Process> readProcesses() = 0;
    virtual int overrideProcessFile(vector<Process> &plist)=0;
    virtual string getExtension()=0;
     virtual string getName()=0;
};

class FileHandler : public IFileHandler {
   
public:
    string extension;
    string  name ;
    string cmd ;
    string filePat ;
public:

    string getExtension() override
    {
        return extension;
    }
    string getName() override
    {
        return name;
    }


    bool fileExists(const string &fileName) override
     {

          filesystem::path fileloc(fileName);
          string folderPath = fs::current_path() ; // Specify the folder path (current directory)

            // Check if the provided path is absolute
            if (fileloc.is_absolute()) 
            {
                
               
                if (fs::exists(fileloc) && fs::is_regular_file(fileloc))
                    {
                        extension = fileloc.extension().string();
                        filePat = fileloc.string();
                        name = fileloc.stem().string();
                        cout << "File found: " << fileloc.filename() << endl;
                        cout << "Extension: " << extension << endl;
                    
                        return true;
                    }
                    
                    cout << "File not found Regularly Searching without extension" << endl;
                    folderPath = fileloc.parent_path();
                    cout<<folderPath<<endl;
            
            }
                    
            
           
             // Iterate through files in the folder
    
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    std::string fullFileName = entry.path().filename().string();
                    std::string nameWithoutExt = entry.path().stem().string(); // File name without extension
                    //nameWithoutExt == fileName
                    if (nameWithoutExt == fileloc.stem().string()) {
                        extension = entry.path().extension().string(); // File extension

                        filePat = folderPath+"/"+fullFileName;
                        cout<<filePat<<endl;
                        name = nameWithoutExt;
                        std::cout << "File found: " << fullFileName << std::endl;
                        std::cout << "Extension: " << extension << std::endl;
                        return true;
                    }
                }
            }
    
            

           return false;

       /* 
        fs::path filePath = fs::current_path() / fileName;

        if (fs::exists(filePath) && fs::is_regular_file(filePath))
         {
            extension = filePath.extension().string();
            filePat = filePath.string();
            name = fileName;
            cout << "File found: " << filePath.filename() << endl;
            cout << "Extension: " << extension << endl;
           
            return true;
        }
        cout << "File not found!" << endl;
        return false;
        */
    }

    int saveProcess(string pid) override
   {
     string processId = pid; 
   
    // Open the file for appending
    ofstream outFile("processes.txt", ios::app);  // 'ios::app' opens the file in append mode
    if (outFile.is_open()) {
        outFile << processId << "#" << name<<"#" << cmd<< "#"<< filePat <<"#"<< endl;  // Write process ID and file path
       // outFile << processId  << endl;  // Write process ID and file path
        outFile.close();
        cout << "Process info saved." << endl;
    } else {
        cout << "Unable to open file for writing!" << endl;
    }

    return 0;
   }

   int overrideProcessFile(vector<Process> &plist) override
   {
    
    // Open the file for overriding
    ofstream outFile("processes.txt");  // 'ios::app' opens the file in append mode
    if (outFile.is_open()) {
        for(const Process p : plist)
        {
            outFile << p.processID << "#" << p.name<<"#" << p.cmd<< "#"<< p.filePath <<"#" << endl;  // Write process ID and file path
       // outFile << processId  << endl;  // Write process ID and file path
        
        }
        outFile.close();
        cout << "Processes info saved." << endl;
    } else {
        cout << "Unable to open file for writing!" << endl;
    }

    return 0;
   }


   vector<Process> readProcesses() override
    {
    vector<Process> processList;

   /*ifstream inFile("processes.txt");
    if (inFile.is_open()) {
        string processId;
        string name;
        string cmd;
        string filePath;
        
        // Read each line and extract the process ID and file path
        while (inFile >> processId >>name >>cmd >> filePath) {
            processList.push_back(Process(processId,name,cmd,filePath));
            cout << "Process ID: " << processId << ", File Path: " << filePath << endl;
        }

        inFile.close();
        */

        ifstream inFile("processes.txt");
        if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string processId, name, cmd, filePath;

            // Read fields separated by commas
            if (getline(ss, processId, '#') &&
                getline(ss, name, '#') &&
                getline(ss, cmd, '#') &&
                getline(ss, filePath, '#')) {
                processList.push_back(Process(processId, name, cmd, filePath));
                cout << "Process ID: " << processId << ", File Path: " << filePath << endl;
            }
        }
        inFile.close();
        }
        else {
            cout << "Unable to open file for reading!" << endl;
        }

    return processList;
   }
};


// Abstraction for Command Execution
class ICommandExecutor {
public:
    virtual pid_t execute(const string &command, const vector<string> &args,const string &name) = 0;
};

class CommandExecutor : public ICommandExecutor {
public:
   
   pid_t execute(const string &command, const vector<string> &args, const string &name) override {
    string file;
    for (const string &s : args) {
        size_t start = s.find(".");
        if (start != string::npos) {
            std::string sub = s.substr(0, start);
            file = sub;
            cout << sub << endl;
        } else {
            cout << "No '.' found in argument: " << s << endl;
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

        dup2(outFd, STDOUT_FILENO);
        dup2(errFd, STDERR_FILENO);
        close(outFd);
        close(errFd);

        vector<char*> c_args;
        for (const auto &arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        if (execvp(command.c_str(), c_args.data()) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}

};


// Process Management
class IProcessManager {
public:
    virtual void listProcesses() = 0;
    virtual void stopProcess(const string &pid) = 0;
    virtual void contProcess(const string &pid) = 0;
    virtual void killProcess(const string &pid) = 0;
    virtual void restartProcess(const string &pid) = 0;
    virtual void addProcess(const string &pid) =0;
    virtual void monitorProcesses() =0;
    
};



class ProcessManager : public IProcessManager {
private:
    vector<Process> processList;
    IFileHandler &fileHandler;

 public:
    ProcessManager(IFileHandler &file):fileHandler(file){};  

   
    string getColoredProcessStatus(const string &state) {
        if (state == "R") return GREEN " Running " RESET;
        if (state == "S") return YELLOW " Sleeping " RESET;
        if (state == "D") return CYAN " Waiting " RESET;
        if (state == "T") return RED " Stopped " RESET;
        if (state == "Z") return CYAN " Zombie " RESET;
        return WHITE " Unknown " RESET;
    }

public:
    void addProcess(const string &pid) override {
      //  processList.push_back(pid);
      fileHandler.saveProcess(pid);
    }

     // Function to extract process name from /proc/<pid>/cmdline
   string getProcessName(const string &pid)
 {
    string cmdlineFile = "/proc/" + pid + "/cmdline";
    ifstream cmdlineStream(cmdlineFile);
    
    if (!cmdlineStream.is_open()) {
        return "Unknown";
    }

    string cmdline;
    getline(cmdlineStream, cmdline, '\0'); // Read the entire cmdline
    cmdlineStream.close();

    if (cmdline.empty()) {
        return "Unknown";
    }

    // Find the last part of the command (e.g., "demo.js" from "node demo.js")
    size_t lastSlash = cmdline.find_last_of('/');
    string processName = lastSlash != string::npos ? cmdline.substr(lastSlash + 1) : cmdline;

    // Remove file extensions if needed (e.g., ".js")
    size_t dotPosition = processName.find_last_of('.');
    if (dotPosition != string::npos) {
        processName = processName.substr(0, dotPosition);
    }

    return processName;
}

    void displayProcessInfo(const Process &p) 
    {
    string statFile = "/proc/" + p.processID + "/stat";
    ifstream statStream(statFile);
    
    if (!statStream.is_open()) {
        cout << "Error opening stat file for PID " << p.processID << endl;
        return;
    }

    string line;
    getline(statStream, line);
    
    // Parsing the stat file (fields we're interested in)
    stringstream ss(line);
    string comm, state;
    long pidInt, ppid, vsize, rss, utime, stime, starttime;
    ss >> pidInt >> comm >> state >> ppid >> vsize >> rss >> utime >> stime >> starttime;
    
    // Clean up the command name (remove parentheses)
    comm = comm.substr(1, comm.size() - 2);

    // Get process name from cmdline
    string processName = p.name;

    // Calculate total CPU time (user + system time)
    long totalTime = utime + stime;

    // Convert starttime to uptime in seconds
    long sysUptime;
    ifstream uptimeFile("/proc/uptime");
    uptimeFile >> sysUptime;
    long uptime = sysUptime - starttime / sysconf(_SC_CLK_TCK);

    // Set process state color
    string stateColor = (state == "R" ? GREEN : (state == "S" ? YELLOW : RED));
    string processtatus = getColoredProcessStatus(state);
    // Output the process information with styling
    // Using fixed width and formatting for columns
    cout << "| " 
         << left << setw(7) << p.processID << " | "  // PID
         << left << setw(40) << processName << " | "  // Command
         << left << setw(7) << processtatus << " | "  // State
         << right << setw(20) << rss * sysconf(_SC_PAGE_SIZE) / 1024 << " KB" << " | "  // Memory
         << right << setw(15) << totalTime << " ticks" << " | "  // CPU Time
         << right << setw(8) << uptime << "s" << " |"  // Uptime
         
         << endl;
}

    void listProcesses() override {
        // Your logic for listing processes, his remains similar to your original code
      
    
      processList = fileHandler.readProcesses();
      if(processList.size()<1)
      {
        cout<<"No avtive processes"<<endl;
        return;
      }

         // Print table header with borders
    cout << "+"<< setw(7)<<"---------"<<"+"<<setw(40)<<"------------------------------------------"<<"+"<<setw(10)<<"------------"<<"+"<<setw(20)<<"-------------------------"<<"+"<<setw(15)<<"-----------------------"<<"+"<<setw(8)<<"-----------" <<"+"<< endl;
   //cout << "|"<< setw(7)<<"PID   "<<"|"<<setw(40)<<"COMMAND"<<"|"<<setw(10)<<"STATE"<<"|"<<setw(20)<<"MEMORY"<<"|"<<setw(15)<<"CPU TIME"<<"|"<<setw(8)<<"UPTIME" <<"|"<< endl;
     cout << "|"<< setw(7)<<"   PID   "<<"|"<<setw(40)<<"                    NAME                  "<<"|"<<setw(10)<<"    STATE   "<<"|"<<setw(20)<<"           MEMORY        "<<"|"<<setw(15)<<"         CPU TIME      "<<"|"<<setw(8)<<"  UPTIME   " <<"|"<< endl;
     cout << "+"<< setw(7)<<"---------"<<"+"<<setw(40)<<"------------------------------------------"<<"+"<<setw(10)<<"------------"<<"+"<<setw(20)<<"-------------------------"<<"+"<<setw(15)<<"-----------------------"<<"+"<<setw(8)<<"-----------" <<"+"<< endl;
  
    /*while ((entry = readdir(dir)) != nullptr) {
        // Check if the entry is a numeric directory (indicating a process)
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            string pid = entry->d_name;
            displayProcessInfo(pid);
        }
    }
*/
        for( Process p : processList)
        {
            //string pid_str = to_string(static_cast<int>(pid)); // Convert pid_t to string
            displayProcessInfo(p);
        
        }
 
    // End the table with a border
    cout << "+"<< setw(7)<<"---------"<<"+"<<setw(40)<<"------------------------------------------"<<"+"<<setw(10)<<"------------"<<"+"<<setw(20)<<"-------------------------"<<"+"<<setw(15)<<"-----------------------"<<"+"<<setw(8)<<"-----------" <<"+"<< endl;
  
    }

    void stopProcess(const string &fileName) override {
    processList = fileHandler.readProcesses();
    for (const auto &process : processList) {
        if (process.name == fileName) {
            pid_t pid = stoi(process.processID);
            if (kill(pid, SIGSTOP) == -1) {
                perror("Error stopping process");
            } else {
                cout << "Process " << fileName << " successfully stopped." << endl;
            }
            return;
        }
    }
    cout << "Process " << fileName << " not found in process list." << endl;
}

    
    
     void contProcess(const string &fileName) override {
    processList = fileHandler.readProcesses();
    for (const auto &process : processList) {
        if (process.name == fileName) {
            pid_t pid = stoi(process.processID);
            if (kill(pid, SIGCONT) == -1) {
                perror("Error continuing process");
            } else {
                cout << "Process " << fileName << " successfully resumed." << endl;
            }
            return;
        }
    }
    cout << "Process " << fileName << " not found in process list." << endl;
}


    void restartProcess(const string &fileName) override
    {

         processList = fileHandler.readProcesses();
        for (const Process p : processList) {
            if (p.name == fileName) {
                killProcess(fileName);
                cout<<"file path"+p.filePath<<endl;
                runCmdsInBash("./new start "+p.filePath);
                return;
            }
        }
        cout << "Process " << fileName << " not found" << endl;
    
    }

    void killProcess(const string &fileName) override {
    processList = fileHandler.readProcesses();
    if (processList.empty()) {
        cout << "No processes found in the list." << endl;
        return;
    }

    bool processFound = false;
    processList.erase(std::remove_if(processList.begin(), processList.end(),
        [&](const Process &process) {
            if (process.name == fileName) {
                processFound = true;
                pid_t pid = stoi(process.processID);
                if (kill(pid, SIGKILL) == -1) {
                    if (errno == ESRCH) {
                        cout << "Process not found: " << pid << endl;
                    } else {
                        perror("Error killing process");
                    }
                } else {
                    cout << "Process " << fileName << " with PID: " << pid << " successfully killed." << endl;
                }
                return true;  // Remove this process from the list
            }
            return false;
        }), processList.end());

    if (!processFound) {
        cout << "Process " << fileName << " not found in process list." << endl;
    } else {
        fileHandler.overrideProcessFile(processList);
    }
}


    void runCmdsInBash(const string &cmd) {
        int result = system(cmd.c_str());
        if (result != 0) {
            cout << "Command failed!" << endl;
        }
    }

    void monitorProcesses() override
    {
        int s =1;
        //processList = fileHandler.readProcesses();
        while (s>0) {
        processList = fileHandler.readProcesses();
        s= processList.size();
        for (Process &process : processList) {
            int status;
            pid_t result = waitpid(stoi(process.processID), &status, WNOHANG); // Non-blocking

            if (result == -1) {
                std::cerr << "Error with waitpid for process " << process.name << std::endl;
            } else if (result == 0) {
                // Process is still running
                continue;
            } else {
                // Process has terminated
                if (WIFEXITED(status)) {
                    std::cout << "Process " << process.name << " (PID: " << process.processID << ") exited normally with status " << WEXITSTATUS(status) << std::endl;
                } else if (WIFSIGNALED(status)) {
                    std::cout << "Process " << process.name << " (PID: " << process.processID << ") terminated by signal " << WTERMSIG(status) << std::endl;
                }

                // Restart the process if it has terminated
                std::cout << "Process " << process.name << " is not running. Restarting..." << std::endl;
                //process.pid = startProcess(process.command);
                std::cout << "Process " << process.name << " restarted with PID: " << process.processID << std::endl;
            }
        }

        sleep(5);  // Check every 5 seconds
    }
    }
};


// Extension Handler
class IExtensionHandler {
public:
    virtual void handleExtension(const string &extension, const string &fileName,const string &name, ICommandExecutor &executor, IProcessManager &processManager) = 0;
};

class ExtensionHandler : public IExtensionHandler {
public:
    void handleExtension(const string &extension, const string &fileName,const string &name, ICommandExecutor &executor, IProcessManager &processManager) override {
        
        if (extension == ".js") {
            if (isInstalled("node")) {
                cout<<"Installed";
                vector<string> args = {"node", fileName+extension};
                pid_t pid = executor.execute("node", args,name);
                processManager.addProcess(to_string(pid));
                
            }
        }
        else if (extension == ".class") {
            if (isInstalled("java")) {
                vector<string> args ;
                string fileNa = fileName;
                if( isAbsolute(fileName)) 
                {
                        // Find the substring "/Jv1"
                    size_t found = fileNa.find(""+name);

                    // If "/Jv1" is found, erase it and everything after it
                    if (found != std::string::npos) {
                        fileNa.erase(found);  // Erase from "/Jv1" onwards
                        args = {"java", "-cp", fileNa, name};
                    }
                } 
                else {
                   args={ "java", fileName};
                    }
               
                pid_t pid = executor.execute("java", args,name);
                processManager.addProcess(to_string(pid));
            }
        }
         else if (extension == ".py") {
            if (isInstalled("python3")) {
                vector<string> args = {"python3", fileName+extension};
                pid_t pid = executor.execute("python3", args,name);
                processManager.addProcess(to_string(pid));
            }
        }
         else if (extension == "") {
            if (true) {
                
                string  cmd = isAbsolute(fileName) ? fileName : "./"+fileName;
                 
                 vector<string> args = {fileName+extension};
                
                pid_t pid = executor.execute(cmd,args,name);
                processManager.addProcess(to_string(pid));
            }
        }
        // Add more extensions if needed
    }

    bool isInstalled(const string &program) {
        string command = "which " + program + " > /dev/null 2>&1";
        return (system(command.c_str()) == 0);
    }
    bool isAbsolute(string f)
    {
         filesystem::path fileloc(f);
        return fileloc.is_absolute() ? true :false ;
           

    }
};

// Main Application
class ProcessApplication {
private:
    ICommandExecutor &executor;
    IFileHandler &fileHandler;
    IProcessManager &processManager;
    IExtensionHandler &extensionHandler;
 
 
public:
    ProcessApplication(ICommandExecutor &exec, IFileHandler &file, IProcessManager &process, IExtensionHandler &ext)
        : executor(exec), fileHandler(file), processManager(process), extensionHandler(ext) {}

    void run(int args, char* arg[]) {

        if (args < 3) {
        cerr << RED "Error: Insufficient arguments provided." RESET << endl;
        cerr << "Usage: ./application <command> <file>" << endl;
        return;
    }
        string cmd = arg[1];
        string fileName = arg[2];

         size_t start = fileName.find(".");    // Find the position of "in"
         std::string sub = fileName.substr(0, start);  // Extract "in"
         fileName=sub;
            
        
        if (cmd == "start") {
            if (fileHandler.fileExists(fileName)) {
                //string extension = fs::path(fileName).extension().string();
                string extension= fileHandler.getExtension();
                string name = fileHandler.getName();
                //cout<<fileHandler.getExtension()<<endl;
                extensionHandler.handleExtension(extension, fileName,name, executor, processManager);
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
        
    
};




// Main function
int main(int argc, char* argv[]) {

   
    CommandExecutor executor;
    FileHandler fileHandler;
    ProcessManager processManager(fileHandler);
    ExtensionHandler extensionHandler;
    
    ProcessApplication app(executor, fileHandler, processManager, extensionHandler);
    app.run(argc, argv);
 
    return 0;
}
