#include "SysInfo.h"

std::string SysInfo::getCpuPercent() const 
{
  return this->cpuPercent;
}

std::string SysInfo::getMemPercent() const 
{
  return std::to_string(this->memPercent);
}

long SysInfo::getUpTime() const 
{
  return this->upTime;
}

std::string SysInfo::getKernelVersion() const 
{
  return this->kernelVer;
}

std::string SysInfo::getTotalProc() const 
{
  return std::to_string(this->totalProc);
}

std::string SysInfo::getRunningProc() const 
{
  return std::to_string(this->runningProc);
}

std::string SysInfo::getThreads() const 
{
  return std::to_string(this->threads);
}

std::string SysInfo::getOsName() const 
{
  return this->osName;
}

void SysInfo::setLastCpuMeasures() 
{
  this->lastCpuStats = ProcessParser::getSysCpuPercent();
}

void SysInfo::getOtherCores(int _size)
{
  this->coresStats = std::vector<std::string>();
  this->coresStats.resize(_size);
  this->lastCpuCoresStats = std::vector<std::vector<std::string>>();
  this->lastCpuCoresStats.resize(_size);
  this->currentCpuCoresStats = std::vector<std::vector<std::string>>();
  this->currentCpuCoresStats.resize(_size);
  for (int i = 0; i < _size; i++) {
    this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(std::to_string(i));
  }
}

void SysInfo::setCpuCoresStats()
{
  for (int i = 0; i < this->currentCpuCoresStats.size(); i++) {
    this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(std::to_string(i));
  }
  for (int i = 0; i < this->currentCpuCoresStats.size(); i++) {
    this->coresStats[i] = ProcessParser::printCpuStats(this->lastCpuCoresStats[i], this->currentCpuCoresStats[i]);
  }
  this->lastCpuCoresStats = this->currentCpuCoresStats;
}

void SysInfo::setAttributes()
{
  this->memPercent = ProcessParser::getSysRamPercent();
  this->upTime = ProcessParser::getSysUpTime();
  this->totalProc = ProcessParser::getTotalNumberOfProcesses();
  this->runningProc = ProcessParser::getNumberOfRunningProcesses();
  this->threads = ProcessParser::getTotalThreads();
  this->currentCpuStats = ProcessParser::getSysCpuPercent();
  this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats, this->currentCpuStats);
  this->lastCpuStats = this->currentCpuStats;
  this->setCpuCoresStats();
}

std::vector<std::string> SysInfo::getCoresStats() const
{
  std::vector<std::string> result = std::vector<std::string>();
  for (int i = 0; i < this->coresStats.size(); i++) {
    std::string temp = ("cpu" + std::to_string(i) + ": ");
    float check = std::stof(this->coresStats[i]);
    if (!check || this->coresStats[i] == "nan") {
      return std::vector<std::string>();
    }
    temp += Util::getProgressBar(this->coresStats[i]);
    result.push_back(temp);
  }
  return result;
}
