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
//#include <constants.h>

#include "Path.h"
#include "Util.h"

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
    static string getOsName();
    static std::string printCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
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

#endif
