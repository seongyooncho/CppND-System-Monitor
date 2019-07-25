#include "ncurses_display.h"
#include "system.h"
#include <cstring>
#include <iostream>
#include "Path.h"
#include "Util.h"
#include "ProcessParser.h"

int main() {
  std::cout << "listing: " + Path::basePath() << std::endl;
  std::string command = "ls " + Path::basePath();
  int n = command.length();
  char char_command[n+1];
  strcpy(char_command, command.c_str());
  std::system(char_command);

  std::cout << Util::convertToTime(403909) << std::endl;
  std::cout << Util::getProgressBar("99%") << std::endl;

  std::cout << ProcessParser::getVmSize("1011") << std::endl;
  std::cout << ProcessParser::getCpuPercent("1011") << std::endl;
  std::cout << ProcessParser::getProcUser("1011") << std::endl;

  auto pids = ProcessParser::getPidList();
  for (auto p : pids) {
    std::cout << p << " ";
  } 
  std::cout << std::endl;

  return 0;

  System system;
  NCursesDisplay::Display(system);
}
