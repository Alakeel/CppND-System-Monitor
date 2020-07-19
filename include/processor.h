#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"
#include "vector"

class Processor {
 public:
  float Utilization();  // DONE: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
  float prev_user{0.0}; //initialize to 0
  float prev_nice{0.0};
  float prev_system{0.0};
  float prev_idle{0.0};
  float prev_iowait{0.0};
  float prev_irq{0.0};
  float prev_softirq{0.0};
  float prev_steal{0.0};
};

#endif
