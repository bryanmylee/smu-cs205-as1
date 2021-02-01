#ifndef MANAGER_H
#define MANAGER_H

#include <stdbool.h>
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
bool manager_handle_run_available(Manager *manager);
bool manager_handle_process_exit(Manager *manager, pid_t pid);
void manager_list(Manager *manager);

#endif

