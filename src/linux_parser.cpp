#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "linux_parser.h"

#include<iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::unordered_map<std::string, float> memory_info;
  string line;
  string key;
  float value;
  string unit;

  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()) {
      for (auto i = 0; i < 5; i++){
        std::getline(filestream, line);
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> key >> value >> unit;
        memory_info.insert({key, value});
      } 
  }
  return (memory_info["MemTotal"]-memory_info["MemAvailable"])/memory_info["MemTotal"]; 
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line, uptime_str;

  std::ifstream filestream(kProcDirectory+kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_str;
  }
  return std::stol(uptime_str); 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  long utime = 0, stime = 0, cutime = 0, cstime = 0;
  utime = std::stol(values[13]);
  stime = std::stol(values[14]);
  cutime = std::stol(values[15]);
  cstime = std::stol(values[16]);
  return (utime+stime+cutime+cstime)/sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto jiffies = LinuxParser::CpuUtilization();

  return std::stol(jiffies[CPUStates::kUser_]) + std::stol(jiffies[CPUStates::kNice_]) + std::stol(jiffies[CPUStates::kSystem_]) + std::stol(jiffies[CPUStates::kIRQ_]) +
         std::stol(jiffies[CPUStates::kSoftIRQ_]) + std::stol(jiffies[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto jiffies = LinuxParser::CpuUtilization();
  
  return std::stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, cpu_info, cpu_name;
  std::vector<std::string> jiffeis;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    linestream >> cpu_name;
    while(linestream >> cpu_info){
      jiffeis.push_back(cpu_info);
    }
  }
  return jiffeis;
}


int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "processes") {
          return value;
        }
    }
  }
  return value; 
}

int LinuxParser::RunningProcesses() {
    string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "procs_running") {
          return value;
        }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string command;
  std::string path = kProcDirectory+std::to_string(pid)+kCmdlineFilename;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    std::getline(filestream, command);
  }
  return command; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  const float kb_to_mb = 1000;
  std::string line;
  std::string key;
  long int value;
  std::string path = kProcDirectory+std::to_string(pid)+kStatusFilename;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          value /= kb_to_mb;
          return std::to_string(value);
        }
      }
    }
  }
  return std::to_string(value);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string line;
  std::string key;
  std::string value;
  std::string path = kProcDirectory+std::to_string(pid)+kStatusFilename;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  auto uid = LinuxParser::Uid(pid);
  std::string line;
  std::string username;
  std::string password;
  std::string user_id;
  std::ifstream filestream(LinuxParser::kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> password >> user_id) {
        if (user_id == uid) {
          return username;
        }
      }
    }
  }
  return username; 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string placeholder, line;
  long int up_time_clock_ticks{0};
  std::string path = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream filestream(path);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++){
      linestream >> placeholder;
    }
    linestream >> up_time_clock_ticks;
  }
  return up_time_clock_ticks/sysconf(_SC_CLK_TCK);
}