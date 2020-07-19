#include "linux_parser.h"
#include "process.h"

#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process (int pid): _pid(pid) {
  Process::CpuUtilization();
  _user     = LinuxParser::User(_pid);
  _ram      = LinuxParser::Ram(_pid);
  _uptime   = LinuxParser::UpTime(_pid);
  _command  = LinuxParser::Command(_pid);
}

// DONE: Return this process's ID
int Process::Pid() { return _pid; }

// DONE: Return this process's CPU utilization
// reference -> https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() {
  std::vector <std::string> utilization = LinuxParser::CpuUtilization(_pid);
  // extract values from system.
  long utime      = stol(utilization[0]);
  long stime      = stol(utilization[1]);
  long cutime     = stol(utilization[2]);
  long cstime     = stol(utilization[3]);
  long starttime  = stol(utilization[4]);
  long hertz      = sysconf(_SC_CLK_TCK);
  long uptime     = LinuxParser::UpTime();
  // determine the total time spent for the process
  long total_time = (utime + stime + cutime + cstime);
  // total elapsed time in seconds since the process started
  long seconds =  uptime - (starttime /  hertz);
  // calculate the CPU usage percentag
  _proc_cpu_utilization = ((float) (total_time/hertz) / (float) seconds);
  return _proc_cpu_utilization;
}

// DONE: Return the command that generated this process
string Process::Command() { return _command; }

// DONE: Return this process's memory utilization
string Process::Ram() { return _ram; }

// DONE: Return the user (name) that generated this process
string Process::User() { return _user; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return _uptime; }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  // return std::stoi(LinuxParser::Ram(a._pid)) < std::stoi(LinuxParser::Ram(_pid));
  // return a._pid < this->_pid; // Update based on latest PID created.
    return a._proc_cpu_utilization < this->_proc_cpu_utilization;
}
