#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "ProcessParser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
  private:
    std::string pid;
    std::string user;
    std::string cmd;
    std::string cpu;
    std::string mem;
    std::string upTime;
  public:
    Process(std::string pid)
    {
      this->pid = pid;
      this->user = ProcessParser::getProcUser(pid);
      this->mem = ProcessParser::getVmSize(pid);
      this->cmd = ProcessParser::getCmd(pid);
      this->upTime = ProcessParser::getProcUpTime(pid);
      this->cpu = ProcessParser::getCpuPercent(pid);
    }
    std::string getProcess();

    int Pid();                             
    std::string User();                    
    std::string Command();                 
    float CpuUtilization();                
    std::string Ram();                     
    long int UpTime();                     
    bool operator<(Process const& a) const;
};

#endif
