#ifndef PROCESS_PARSER_H
#define PROCESS_PARSER_H

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"

#include "util.h"

class ProcessParser {
  public:
    static std::string getCmd(std::string pid);
    static std::vector<std::string> getPidList();
    static std::string getVmSize(std::string pid);
    static std::string getCpuPercent(std::string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(std::string pid);
    static std::string getProcUser(std::string pid);
    static std::vector<std::string> getSysCpuPercent(std::string coreNumber = "");
    static float getSysRamPercent();
    static std::string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static std::string getOsName();
    static std::string printCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
  private:
    static int getNumberOfCores();
    static float get_sys_active_cpu_time(std::vector<std::string> values);
    static float get_sys_idle_cpu_time(std::vector<std::string> values);
};

std::string ProcessParser::getVmSize(std::string pid) {
  std::string line;
  std::string name = "VmData";
  std::string value;
  float result;

  std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      result = (stof(values[1])/float(1024*1024));
      break;
    }
  }
  return std::to_string(result);
}

std::string ProcessParser::getCpuPercent(std::string pid) {
  std::string line;
  std::string value;
  float result;

  std::ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  std::getline(stream, line);
  std::istringstream buf(line);
  std::istream_iterator<std::string> beg(buf), end;
  std::vector<std::string> values(beg, end);

  float utime = stof(ProcessParser::getProcUpTime(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  result = 100.0 * ((total_time / freq) / seconds);

  return std::to_string(result);
}

std::string ProcessParser::getProcUpTime(std::string pid) {
  std::string line;
  std::string value;
  float result;

  std::ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  std::getline(stream, line);
  std::istringstream buf(line);
  std::istream_iterator<std::string> beg(buf), end;
  std::vector<std::string> values(beg, end);

  float freq = sysconf(_SC_CLK_TCK);
  result = stof(values[13]) / freq;

  return  std::to_string(result);
}

long int ProcessParser::getSysUpTime() {
  std::string line;
  std::string value;
  std::ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
  std::getline(stream, line);
  std::istringstream buf(line);
  std::istream_iterator<std::string> beg(buf), end;
  std::vector<std::string> values(beg, end);

  return  stoi(values[0]);
}

std::string ProcessParser::getProcUser(std::string pid) {
  std::string line;
  std::string name = "Uid:";
  std::string result = "";

  std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      name = "x:" + values[1];
      break;
    }
  }
  
  stream = Util::getStream("/etc/passwd");
  while(std::getline(stream, line)) {
    if (line.find(name) != std::string::npos) {
      result = line.substr(0, line.find(":"));
      break;
    }
  }

  return result;

}

std::vector<std::string> ProcessParser::getPidList() {
  DIR* dir;
  std::vector<std::string> container;
  if (!(dir = opendir("/proc")))
    throw std::runtime_error(std::strerror(errno));

  while(dirent* dirp = readdir(dir)) {
    if (dirp->d_type != DT_DIR)
      continue;
    if (std::all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){return std::isdigit(c);})) {
      container.push_back(dirp->d_name);
    }
  }
  if(closedir(dir))
    throw std::runtime_error(std::strerror(errno));
  return container;

}

std::string ProcessParser::getCmd(std::string pid) {
  std::string line;
  std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
  std::getline(stream, line);
  return line;
}

int ProcessParser::getNumberOfCores() {
  std::string line;
  std::string name = "cpu cores";

  std::ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      return stoi(values[3]);
    }
  }
  return 0;
}

std::vector<std::string> ProcessParser::getSysCpuPercent(std::string coreNumber) {
  std::string line;
  std::string name = "cpu" + coreNumber;

  std::ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      return values;
    }
  }
  return (std::vector<std::string>());
}

float ProcessParser::get_sys_active_cpu_time(std::vector<std::string> values) {
  return (stof(values[S_USER]) +
          stof(values[S_NICE]) +
          stof(values[S_SYSTEM]) +
          stof(values[S_IRQ]) +
          stof(values[S_SOFTIRQ]) +
          stof(values[S_STEAL]) +
          stof(values[S_GUEST]) +
          stof(values[S_GUEST_NICE]));
}
float ProcessParser::get_sys_idle_cpu_time(std::vector<std::string> values) {
  return (stof(values[S_IDLE]) +
          stof(values[S_IOWAIT]));
}

std::string ProcessParser::printCpuStats(std::vector<std::string> values1, std::vector<std::string>values2) {
  float activeTime = get_sys_active_cpu_time(values2) - get_sys_active_cpu_time(values1);
  float idleTime = get_sys_idle_cpu_time(values2) - get_sys_idle_cpu_time(values1);
  float totalTime = activeTime + idleTime;
  float result = 100.0 * (activeTime / totalTime);
  return std::to_string(result);
}

float ProcessParser::getSysRamPercent() {
  std::string line;
  std::string name1 = "MemAvailable:";
  std::string name2 = "MemFree:";
  std::string name3 = "Buffers:";
  std::string value;

  std::ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
  float total_mem = 0;
  float free_mem = 0;
  float buffers = 0;
  while(std::getline(stream, line)) {
    if (total_mem != 0 && free_mem != 0)
      break;
    if (line.compare(0, name1.size(), name1) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      total_mem = std::stof(values[1]);
    }
    if (line.compare(0, name2.size(), name2) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      free_mem = std::stof(values[1]);
    }
    if (line.compare(0, name3.size(), name3) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      buffers = std::stof(values[1]);
    }
  }
  return float(100.0 * (1 - (free_mem/(total_mem - buffers))));
}

std::string ProcessParser::getSysKernelVersion() {
  std::string line;
  std::string name = "Linux version ";
  std::ifstream stream = Util::getStream((Path::basePath() + Path::versionPath()));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      return values[2];
    }
  }
  return "";
}

int ProcessParser::getTotalThreads() {
  std::string line;
  int result = 0;
  std::string name = "Threads:";
  std::vector<std::string>_list = ProcessParser::getPidList();
  for (int i = 0; i < _list.size(); i++) {
    std::string pid = _list[i];
    std::ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    while(std::getline(stream, line)) {
      if (line.compare(0, name.size(), name) == 0) {
        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> values(beg, end);
        result += std::stoi(values[1]);
      }
    }
  }
  return result;
}

int ProcessParser::getTotalNumberOfProcesses() {
  std::string line;
  int result = 0;
  std::string name = "processes";
  std::ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

int ProcessParser::getNumberOfRunningProcesses() {
  std::string line;
  int result = 0;
  std::string name = "procs_running";
  std::ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> values(beg, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

std::string ProcessParser::getOsName() {
  std::string line;
  std::string name = "PRETTY_NAME=";
  std::ifstream stream = Util::getStream(("/etc/os-release"));
  while(std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::size_t found = line.find("=");
      found++;
      std::string result = line.substr(found);
      result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
      return result;
    }
  }
  return "";
}

#endif
