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

  Util u;
  std::cout << u.convertToTime(403909) << std::endl;
  std::cout << u.getProgressBar("99%") << std::endl;

  ProcessParser p;
  std::cout << p.getVmSize("1003") << std::endl;

  return 0;

  System system;
  NCursesDisplay::Display(system);
}
