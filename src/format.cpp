#include "format.h"

#include <string>
#include <iomanip>

using std::string;

#define SEC_IN_HR 3600
#define SEC_IN_MIN 60

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long int seconds) {
  long int hours, minutes;
  hours = seconds / SEC_IN_HR;
  minutes = (seconds % SEC_IN_HR) / SEC_IN_MIN;
  seconds = seconds % SEC_IN_MIN;
  std::ostringstream stream;
  stream << std::setw(2) << std::setfill('0') << hours   << ":"
         << std::setw(2) << std::setfill('0') << minutes << ":"
         << std::setw(2) << std::setfill('0') << seconds;
  return stream.str();
}
