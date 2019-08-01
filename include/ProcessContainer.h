#ifndef PROCESS_CONTAINER_H
#define PROCESS_CONTAINER_H

#include <string>
#include <vector>
#include "process.h"

class ProcessContainer {
  public:
    ProcessContainer()
    {
      this->refreshList();
    }
    void refreshList();
    std::string printList();
    std::vector<std::string> getList();
  private:
    std::vector<Process> _list;
};

#endif
