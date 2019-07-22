#include "ncurses_display.h"
#include "system.h"
#include <cstring>
#include <iostream>
#include "Path.h"

int main() {
  std::cout << "listing: " + Path::basePath() << std::endl;
  std::string command = "ls " + Path::basePath();
  int n = command.length();
  char char_command[n+1];
  strcpy(char_command, command.c_str());
  std::system(char_command);
  return 0;

  System system;
  NCursesDisplay::Display(system);
}
