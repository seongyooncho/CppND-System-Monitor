#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"


int Process::Pid() { return std::stoi(this->pid); }

float Process::CpuUtilization() { return std::stof(this->cpu); }

std::string Process::Command() { return this->cmd; }

std::string Process::Ram() { return this->mem; }

std::string Process::User() { return this->user; }

long int Process::UpTime() { return std::stol(this->upTime); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }

std::string Process::getProcess()
{
  this->mem = ProcessParser::getVmSize(this->pid);
  this->upTime = ProcessParser::getProcUpTime(this->pid);
  this->cpu = ProcessParser::getCpuPercent(this->pid);
  return (this->pid + "   "
          + this->user
          + "   "
          + this->mem.substr(0, 5)
          + "   "
          + this->cpu.substr(0, 5)
          + "   "
          + this->upTime.substr(0, 5)
          + "   "
          + this->cmd.substr(0, 30)
          + "...");
}
