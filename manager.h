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
bool manager_stop(Manager *manager, pid_t pid);
bool manager_force_resume(Manager *manager, pid_t pid);
bool manager_terminate(Manager *manager, pid_t pid);
void manager_terminate_all(Manager *manager);
void manager_list(Manager *manager);
void manager_poll_processes(Manager *manager);
void manager_reconcile_state(Manager *manager);

#endif

