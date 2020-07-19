#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid); // constructor
  int         Pid();                             // DONE: See src/process.cpp
  float       CpuUtilization();                  // TODO: See src/process.cpp
  std::string Command();                         // DONE: See src/process.cpp
  std::string Ram();                             // DONE: See src/process.cpp
  std::string User();                            // DONE: See src/process.cpp
  long int    UpTime();                          // DONE: See src/process.cpp
  bool        operator<(Process const& a) const; // DONE: See src/process.cpp

  // DONE: Declare any necessary private members
 private:
  int         _pid;
  float       _cpuUtilization;
  std::string _command;
  std::string _ram;
  std::string _user;
  long int    _uptime;

  // CPU UTIL
  long  ProcCpuUsage;
  long  PrevTotalSYSJiffies = 0;
  long  PrevProcJiffies = 0;
  long  ProcJiffies;
  long  TotalSYSJiffies;
  float _proc_cpu_utilization{0};

};

#endif
