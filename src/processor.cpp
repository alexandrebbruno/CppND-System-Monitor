#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  long total = LinuxParser::Jiffies();
  long active = LinuxParser::ActiveJiffies();
  long idle = LinuxParser::IdleJiffies();
  
  long total_old = total_;
  long idle_old = idle_;
  
  float total_delta = total - total_old;
  float idle_delta = idle - idle_old;
  
  float utilization = (total_delta - idle_delta)/total_delta;
  
  // Update
  idle_ = idle;
  total_ = total;
  active_ = active;
  
  return utilization; 
}