#ifndef MANAGER_H
#define MANAGER_H

#include "process.h"

#define RUNNING_SIZE 3

typedef struct manager {
  ProcessQueue *stopped;
  ProcessQueue *running;
  int running_count;
  ProcessQueue *terminated;
} Manager;

Manager *manager_new();
void manager_run(char **arg_list);

#endif

