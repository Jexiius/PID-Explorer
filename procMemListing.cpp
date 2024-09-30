#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <cctype> // For std::isdigit
#include <pwd.h>
#include <cstring>
#include <sys/stat.h> // for stat() for checking PID

class Process
{
private:
    int pid;
    std::string user;
    std::string name;
    std::string cmdLine;
    int pss;
    int rss;

public:
    // constructor
    Process(int pid, const std::string &user, const std::string &name, const std::string &cmdLine, int pss, int rss)
        : pid(pid), user(user), name(name), cmdLine(cmdLine), pss(pss), rss(rss) {}

    // friend function for overloaded output
    friend std::ostream &operator<<(std::ostream &os, const Process &p)
    {
        os << p.user << " " << p.pid << " '" << p.cmdLine << "' '" << p.name << "' " << p.pss << " " << p.rss;
        return os;
    }
};

// prototypes
void parseArgs(int argc, char *argv[], int &pid, std::string &user);
int getRSS(int pid);
int getPSS(int pid);
std::string getProcessName(int pid);
std::string getCmdLine(int pid);
std::string getUserName(int uid);
Process getProcessInfo(int pid, const std::string &user);

int main(int argc, char *argv[])
{
    int pid = 0;
    std::string user = "";
    parseArgs(argc, argv, pid, user);

    //check if PID is possible
    if (pid == 0 && user.empty()){
        std::cerr << "PID not valid " << std::endl;
        exit(95);
    }

    // Check if the PID exists
    std::string path = "/proc/" + std::to_string(pid);
    struct stat sb;
    if (empty(user) && (stat(path.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode)))
    {
        exit(0);
    }

    // Get and print information for the specified process
    std::cout << getProcessInfo(pid, user) << std::endl;
    return 0;
}

void parseArgs(int argc, char *argv[], int &pid, std::string &user) {
    if (argc < 2) {
        std::cerr << "Usage: prog [-u|--user <username>] [-p|--pid <pid>] [<pid>]" << std::endl;
        exit(1);
    }
    for (int i = 1; i < argc; i = i + 2) {
        if (std::string(argv[i]) == "-p" || std::string(argv[i]) == "--pid") {
            try {
                pid = std::stoi(argv[i+1]);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid PID. Please provide a valid integer." << std::endl;
                exit(97);
            }
        }
        else if (std::string(argv[i]) == "-u" || std::string(argv[i]) == "--user") {
            user = std::string(argv[i+1]);
        }
        else{
            pid = std::stoi(argv[1]);
        }
    }
}


int getRSS(int pid) {
    // opening the /proc/[pid]/statm file to read the memory information
    std::ifstream statm("/proc/" + std::to_string(pid) + "/statm");
    int rss = 0;
    if (statm.is_open()) {
        std::string unused;
        // reading and parsing the rss value from the statm file
        statm >> unused >> rss;
        rss *= sysconf(_SC_PAGESIZE) / 1024; // converting pages to kilobytes
    }
    return rss;
}

int getPSS(int pid) {
    // opening the /proc/[pid]/smaps file to read the memory information
    std::ifstream smaps("/proc/" + std::to_string(pid) + "/smaps");
    int pss = 0;
    std::string line;
    // parsing each line to find and sum up "Pss" values
    while (std::getline(smaps, line)) {
        std::istringstream ss(line);
        std::string key;
        int value;
        ss >> key >> value;
        if (key == "Pss:") {
            pss += value;
        }
    }
    return pss;
}

std::string getProcessName(int pid) {
    // opening the /proc/[pid]/comm file to read the process name
    std::ifstream comm("/proc/" + std::to_string(pid) + "/comm");
    std::string name;
    std::getline(comm, name); // reading the process name
    return name;
}

std::string getCmdLine(int pid) {
    // opening the /proc/[pid]/cmdline file to read the command line
    std::ifstream cmdline("/proc/" + std::to_string(pid) + "/cmdline");
    std::string line, cmd;
    // reading each null-terminated string from the cmdline
    while (std::getline(cmdline, line, '\0')) {
        if (!cmd.empty()) {
            cmd += " ";
        }
        cmd += line;
    }
    return cmd;
}

std::string getUserName(int uid) {
    // getting the username associated with the given UID
    struct passwd* pw = getpwuid(uid);
    if (pw) {
        return std::string(pw->pw_name); // returning the username
    } else {
        std::cerr << "User with UID '" << uid << "' not found." << std::endl;
        exit(98);
    }
}

Process getProcessInfo(int pid, const std::string &user)
{
    struct stat sb;
    std::string path = "/proc/" + std::to_string(pid);

    // checking if the /proc/[pid] directory exists
    if (stat(path.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
        std::cerr << "PID " << pid << " not found." << std::endl;
        exit(96);
    }

    // retrieving the process information
    std::string name = getProcessName(pid);
    std::string cmdLine = getCmdLine(pid);
    std::string currentUser = getUserName(sb.st_uid);
    int rss = getRSS(pid);
    int pss = getPSS(pid);

    // checking if the current user matches the expected user, if provided
    if (!user.empty() && currentUser != user)
    {
        std::cerr << "User mismatch for PID " << pid << ". Expected: " << user << ", found: " << currentUser << std::endl;
        exit(99);
    }

    // creating a Process object with the retrieved information
    return Process(pid, currentUser, name, cmdLine, pss, rss);
}