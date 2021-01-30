#ifndef MANAGER_H
#define MANAGER_H

#include <time.h>
#include "process.h"

#define RUNNING_SIZE 3

typedef struct manager {
  ProcessQueue *stopped;
  ProcessQueue *running;
  ProcessQueue *terminated;
} Manager;

Manager *manager_new();
void manager_free(Manager *manager);
void manager_run(Manager *manager, char **arg_list);
void manager_list(Manager *manager);

#endif

