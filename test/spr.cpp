
#include <iostream>
#include <vector>
#include <string>

#include <iostream>
#include <filesystem>
#include <string>

#include <unistd.h>  // For fork(), execlp()
#include <sys/wait.h> // For waitpid()
#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//#include <iostream>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>  
#include <sstream>

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <sys/stat.h>
#include <cstring>
#include <iomanip>

using namespace std;

namespace fs = std::filesystem;
 string extension ;

 vector<string> cmds;
 static vector<string> process;


// ANSI Escape codes for colors and styling
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Function to map state codes to human-readable statuses with colors
string getColoredProcessStatus(const string &state) {
    if (state == "R") return GREEN " Running  "RESET;
    if (state == "S") return YELLOW " Sleeping " RESET;
    if (state == "D") return CYAN " Waiting  " RESET;
    if (state == "T") return RED " Stopped  " RESET;
    if (state == "Z") return CYAN " Zombie   "RESET;
    if (state == "X") return WHITE " Dead     "RESET;
    return WHITE  " Unknown  " RESET;

    
}

// Function to extract process name from /proc/<pid>/cmdline
string getProcessName(const string &pid) {
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


// Function to read process information from /proc/<pid>/stat
void displayProcessInfo(const string &pid) {
    string statFile = "/proc/" + pid + "/stat";
    ifstream statStream(statFile);
    
    if (!statStream.is_open()) {
        cout << "Error opening stat file for PID " << pid << endl;
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
    string processName = getProcessName(pid);

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
         << left << setw(7) << pid << " | "  // PID
         << left << setw(40) << processName << " | "  // Command
         << left << setw(7) << processtatus << " | "  // State
         << right << setw(20) << rss * sysconf(_SC_PAGE_SIZE) / 1024 << " KB" << " | "  // Memory
         << right << setw(15) << totalTime << " ticks" << " | "  // CPU Time
         << right << setw(8) << uptime << "s" << " |"  // Uptime
         
         << endl;
}

// Function to list all the PIDs from /proc directory
void listProcesses() {
    DIR *dir = opendir("/proc");
    if (dir == nullptr) {
        cerr << "Failed to open /proc directory" << endl;
        return;
    }

    struct dirent *entry;
    
    // Print table header with borders
    cout << "+"<< setw(7)<<"---------"<<"+"<<setw(40)<<"------------------------------------------"<<"+"<<setw(10)<<"------------"<<"+"<<setw(20)<<"-------------------------"<<"+"<<setw(15)<<"-----------------------"<<"+"<<setw(8)<<"-----------" <<"+"<< endl;
   //cout << "|"<< setw(7)<<"PID   "<<"|"<<setw(40)<<"COMMAND"<<"|"<<setw(10)<<"STATE"<<"|"<<setw(20)<<"MEMORY"<<"|"<<setw(15)<<"CPU TIME"<<"|"<<setw(8)<<"UPTIME" <<"|"<< endl;
     cout << "|"<< setw(7)<<"   PID   "<<"|"<<setw(40)<<"                    COMMAND               "<<"|"<<setw(10)<<"    STATE   "<<"|"<<setw(20)<<"           MEMORY        "<<"|"<<setw(15)<<"         CPU TIME      "<<"|"<<setw(8)<<"  UPTIME   " <<"|"<< endl;
     cout << "+"<< setw(7)<<"---------"<<"+"<<setw(40)<<"------------------------------------------"<<"+"<<setw(10)<<"------------"<<"+"<<setw(20)<<"-------------------------"<<"+"<<setw(15)<<"-----------------------"<<"+"<<setw(8)<<"-----------" <<"+"<< endl;
  
    /*while ((entry = readdir(dir)) != nullptr) {
        // Check if the entry is a numeric directory (indicating a process)
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            string pid = entry->d_name;
            displayProcessInfo(pid);
        }
    }
*/
for(string pid : process)
{
     //string pid_str = to_string(static_cast<int>(pid)); // Convert pid_t to string
     displayProcessInfo(pid);
   
}
 
    // End the table with a border
    cout << "+"<< setw(7)<<"---------"<<"+"<<setw(40)<<"------------------------------------------"<<"+"<<setw(10)<<"------------"<<"+"<<setw(20)<<"-------------------------"<<"+"<<setw(15)<<"-----------------------"<<"+"<<setw(8)<<"----------" <<"+"<< endl;
  
    closedir(dir);
}






 int checkFileExits(string fileName) 
 {
    std::string folderPath = "./"; // Specify the folder path (current directory)
   // std::string fileName;
    
   // std::cout << "Enter the file name (without extension): ";
    //std::cin >> fileName;
   
    // Iterate through files in the folder
    bool fileFound = false;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string fullFileName = entry.path().filename().string();
            std::string nameWithoutExt = entry.path().stem().string(); // File name without extension
             extension = entry.path().extension().string(); // File extension

            if (nameWithoutExt == fileName) {
                fileFound = true;
                std::cout << "File found: " << fullFileName << std::endl;
                std::cout << "Extension: " << extension << std::endl;
                break;
            }
        }
    }

    if (!fileFound) {
        std::cout << "File not found!" << std::endl;
    }
    
    return 0;
}

// Function to check if a command/executable is available
bool isInstalled(const string &program) {
    string command = "which " + program + " > /dev/null 2>&1";
    return (system(command.c_str()) == 0);  // Return true if the command exists
}



pair<string,string> argsParse(int args,char* arg[])
    {
            
            string cmd = arg[1];
            string app = arg[2];
            pair<string,string> r (cmd,app);
            return r;
    }



   int addCmd(string cmd)
   {
    cmds.push_back(cmd);
   }


pid_t runCommandAsSeparateProcess(const char* command, char* const args[]) {
    pid_t pid = fork(); // Create a new process
    
    if (pid == -1) {
        // Error in forking
        perror("fork");
    } else if (pid == 0) {

        // Open output and error log files using low-level file I/O
        int outFd = open("output.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        int errFd = open("error.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        int in = open("input.log", O_WRONLY | O_CREAT | O_APPEND, 0644);;

        if (outFd == -1 || errFd == -1 || in == -1) {
            std::cerr << "Failed to open log files!" << std::endl;
            exit(1);
        }

        // Redirect stdout and stderr to the log files
        dup2(outFd, STDOUT_FILENO); // Redirect stdout to output.log
        dup2(errFd, STDERR_FILENO); // Redirect stderr to error.log
        dup2(in, STDIN_FILENO); 
        // Close the file descriptors after redirecting
        close(outFd);
        close(errFd);

        // Child process: Replace the current process with the new one
        if (execvp(command, args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE); // Exit if execvp fails
        }
    } 
    /*else {
        // Parent process: Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Child exited with status " << WEXITSTATUS(status) << "\n";
        } else {
            std::cout << "Child process did not terminate normally\n";
        }
    }
    */
  return pid ;
}

void runCmdsInBash(string cmd)
{
     // Example Bash command
    
    const char* command = cmd.c_str();
    //const char* command = "node demo.js";
    int result = system(command); // Executes the command
    
    // Check the result
    if (result == 0) {
        // Command succeeded
    } else {
        // Command failed
    }
}

bool checkProcess(string pid)
{
    for(string p:process)
    {
        if(p==pid)return true;
    }
    return false;
}

int main(int args,char* arg[])
{
   pair<string,string> opt = argsParse(args,arg);
     string c = opt.first;
     string s = opt.second;
     if(c=="start")
     {
        

        checkFileExits(s);

        if(extension==".js")
        {
            if(isInstalled("node"))
            {
                const char* command = "node";
                char* const args[] = {const_cast<char*>("node"), const_cast<char*>((s+extension).c_str()), nullptr};


                pid_t p = runCommandAsSeparateProcess(command, args);
                string pid_str = to_string(static_cast<int>(p)); // Convert pid_t to string
                process.push_back(pid_str);
                std::cout<<p;
            }
        }
        else if(extension==".class")
        {
            if(isInstalled("java"))
            {
                const char* command = "java";
                char* const args[] = {const_cast<char*>("java"), const_cast<char*>((s+extension).c_str()), nullptr};


                pid_t p = runCommandAsSeparateProcess(command, args);
                string pid_str = to_string(static_cast<int>(p)); // Convert pid_t to string
                process.push_back(pid_str);
                std::cout<<p;
            }
        }
       else if(extension==".py")
        {
            if(isInstalled("node"))
            {
                const char* command = "node";
                char* const args[] = {const_cast<char*>("node"), const_cast<char*>((s+extension).c_str()), nullptr};


                pid_t p = runCommandAsSeparateProcess(command, args);
                string pid_str = to_string(static_cast<int>(p)); // Convert pid_t to string
                process.push_back(pid_str);
                std::cout<<p;
            }
        }
        else if(extension=="")
        {
            string cv = "./"+s;
          const char* command = cv.c_str();
                char* const args[] = {const_cast<char*>(cv.c_str()), nullptr};


                pid_t p = runCommandAsSeparateProcess(command, args);
                string pid_str = to_string(static_cast<int>(p)); // Convert pid_t to string
                process.push_back(pid_str);
                std::cout<<p;
        }
     }
    else if(c=="list")
    {
        listProcesses();
    }
    else if(c=="stop")
    {
       
    if (checkProcess(s)) 
    {
        runCmdsInBash("kill -STOP "+s);
    } else {
        std::cout << "Process " << s << " not found" << std::endl;
    }
        
    }
    return 0;
}