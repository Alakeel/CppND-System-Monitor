#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

// for bonus question
// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  bool keyFound = false;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (!keyFound && std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          keyFound = true;
        }
      }
    }
  }
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, skip;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> skip >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  // std::cout << pids.size() << std::endl << pids.size() << std::endl;
  return pids;
}

// solution for bonus
// kept empty due to the virtual machine not supports expermental filestream.
// vector<int> LinuxParser::Pids() {
//   vector<int> pids;
//   for(auto const &entry:fs::directory_iterator(kProcDirectory.c_str())) {
//     auto filename = entry.path().filename().generic_string();
//     if (fs::is_directory(entry.status())) {
//       if (std::all_of(filename.begin(), filename.end(), isdigit)) {
//         pids.push_back(std::stoi(filename)); // push pid
//       }
//     }
//   }
//   return pids;
// }

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  float memory{0.0};
  string::size_type sz;
  bool keyFound = false;
  float memTotal, memFree, buffers;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if(filestream.is_open()) {
    while(!keyFound && std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream ss(line);
      ss >> key >> value;
      if (key == "MemTotal"){
        memTotal = stof(value, &sz);
      } else if (key == "MemFree") {
        memFree = stof(value, &sz);
        // return (memTotal - memFree)/ memTotal; // simple way to get mem utilization
      } else if (key == "Buffers"){
        buffers = stof(value, &sz);
        memory = 1.0 - (memFree / (memTotal - buffers));
        keyFound = true;
      }
    }
  }
  filestream.close();
  return memory;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime{0};
  string value_uptime, value_idle, line;
  string::size_type sz;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream ss(line);
      ss >> value_uptime >> value_idle;
      uptime = stol(value_uptime, &sz);
  }
  filestream.close();
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID (NOT USED)
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long value{0};
  string line, utime, stime, cutime, cstime;
  string skip;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream ss(line);
    for (int i= 1 ; i <= 13; ++i) {
      ss >> skip;
    }
    ss >> utime >> stime >> cutime >> cstime;
    value = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
  }
  filestream.close();
  return value;
}

// TODO: Read and return the number of active jiffies for the system (NOT USED)
long LinuxParser::ActiveJiffies() {
  std::vector <std::string> utilization = LinuxParser::CpuUtilization();
  // extract values from system.
  float user    = stof(utilization[0]);
  float nice    = stof(utilization[1]);
  float system  = stof(utilization[2]);
  // float idle    = stof(utilization[3]);
  // float iowait  = stof(utilization[4]);
  float irq     = stof(utilization[5]);
  float softirq = stof(utilization[6]);
  float steal   = stof(utilization[7]);
  return user + nice + system + irq + softirq + steal;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector <std::string> utilization = LinuxParser::CpuUtilization();
  // extract values from system.
  float idle    = stof(utilization[3]);
  float iowait  = stof(utilization[4]);
  return idle + iowait;
 }

// DONE: Read and return CPU utilization (This is used for Single Processor PID)
vector<string> LinuxParser::CpuUtilization(int pid) {
  vector<string> cpu_data;
  string line, utime, stime, cutime, cstime, starttime;
  string skip;
  bool keyFound = false;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    while(!keyFound && std::getline(filestream, line)){
      std::istringstream ss(line);
      for (int i= 1 ; i <= 13; ++i) {
        ss >> skip;
      }
      ss >> utime >> stime >> cutime >> cstime;
      cpu_data.emplace_back(utime);
      cpu_data.emplace_back(stime);
      cpu_data.emplace_back(cutime);
      cpu_data.emplace_back(cstime);
      for (int i= 1 ; i <= 4; ++i) {
        ss >> skip;
      }
      ss >> starttime;
      cpu_data.emplace_back(starttime);
      keyFound = true;
    }
  }
  filestream.close();
  return cpu_data;
}


// DONE: Read and return CPU utilization (This is used for SYSTEM Agg CPU)
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_data;
  string line;
  string key;
  string value;
  bool keyFound = false;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(!keyFound && std::getline(filestream, line)){
      std::istringstream ss(line);
      while (ss >> key >> value){
        if (key == "cpu"){
          for(int i=1; i<=10 ; i++){
            cpu_data.push_back(value);
            ss >> value;
          }
          keyFound = true;
        }
      }
    }
  }
  filestream.close();
  return cpu_data;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  bool keyFound = false;
  int totalProc{0};
  string::size_type sz;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(!keyFound && std::getline(filestream, line)){
      std::istringstream ss(line);
      ss >> key >> value;
      if (key == "processes"){
        totalProc = stoi(value, &sz);
        keyFound = true;
      }
    }
  }
  filestream.close();
  return totalProc;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  bool keyFound = false;
  int runningProc{0};
  string::size_type sz;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(!keyFound && std::getline(filestream, line)){
      std::istringstream ss(line);
      ss >> key >> value;
      if (key == "procs_running"){
        runningProc = stoi(value, &sz);
        keyFound = true;
      }
    }
  }
  filestream.close();
  return runningProc;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmdLine;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream, cmdLine);
    return cmdLine;
  }
  filestream.close();
  return "Unknown";
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value;
  long memUsage{0};
  bool keyFound{false};
  std::stringstream filePath;
  filePath << kProcDirectory << "/" << pid << "/" << kStatusFilename;
  std::ifstream filestream(filePath.str());
  if(filestream.is_open()) {
    while(!keyFound && std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream ss(line);
      ss >> key >> value;
      if (key == "VmSize"){
        memUsage = stol(value) / 1024; // consider converting the memory utilization into megabytes.
        keyFound = true;
      }
    }
  }
  filestream.close();
  return to_string((int) memUsage); // or use float and std::setprecision
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key;
  bool keyFound = false;
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatusFilename;
  std::ifstream filestream(filename.str());
  string uid;
  if (filestream.is_open()) {
    while (!keyFound && std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        keyFound = true;
      }
    }
  }
  filestream.close();
  return uid;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string userName;
  string line;
  bool keyFound = false;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
      while (!keyFound && std::getline(filestream, line)) {
        std::replace(line.begin(), line.end(), ' ', '_');
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        string pwd;
        string currentUid;
        linestream >> userName >> pwd >> currentUid;
        if (currentUid == uid)
          keyFound = true;
      }
  }
  filestream.close();
  return userName;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long starttime{0};
  string line, skip, value_22nd;
  string::size_type sz;
  std::stringstream filePath;
  // set file path of proc PID
  filePath << kProcDirectory << "/" << pid << "/" << kStatFilename;
  std::ifstream filestream(filePath.str());
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i=0; i<21; i++) linestream >> skip;
    linestream >> value_22nd; // start time
    // Note that the "starttime" value in this file is measured in "clock ticks". In order to convert from "clock ticks" to seconds.
      return LinuxParser::UpTime() - (stol(value_22nd, &sz)/sysconf(_SC_CLK_TCK));
  }
  filestream.close();
  return starttime;
}
