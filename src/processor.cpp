#include "processor.h"

using std::stof;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector <std::string> utilization = LinuxParser::CpuUtilization();
  // extract values from system.
  float user    = stof(utilization[0]);
  float nice    = stof(utilization[1]);
  float system  = stof(utilization[2]);
  float idle    = stof(utilization[3]);
  float iowait  = stof(utilization[4]);
  float irq     = stof(utilization[5]);
  float softirq = stof(utilization[6]);
  float steal   = stof(utilization[7]);
//   float guest = stof(utilization[8]);
//   float guest_nice = stof(utilization[9]);

  // Calculations
  // reference: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  float PrevIdle = prev_idle + prev_iowait;
  float Idle = idle + iowait; // LinuxParser::IdleJiffies()

  float PrevNonIdle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
  float NonIdle = user + nice + system + irq + softirq + steal; // LinuxParser::ActiveJiffies()

  float PrevTotal = PrevIdle + PrevNonIdle;
  float Total = Idle + NonIdle;

  // differentiate: actual value minus the previous one
  float totald = Total - PrevTotal;
  float idled = Idle - PrevIdle;

  // final output
  float cpuPercentage = (totald - idled) / totald;

  prev_user = user;
  prev_nice = nice;
  prev_system = system;
  prev_idle = idle;
  prev_iowait = iowait;
  prev_irq = irq;
  prev_softirq = softirq;
  prev_steal = steal;

  return cpuPercentage;
}
