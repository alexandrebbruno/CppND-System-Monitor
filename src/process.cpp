#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid): pid_(pid){
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid);
  uptime_ = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
  user_ = LinuxParser::User(pid);
  utilization_ = float(LinuxParser::ActiveJiffies(pid)) / float(uptime_);
};

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { return utilization_; }

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { return ram_; }

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return (utilization_ < a.utilization_)?true:false; }