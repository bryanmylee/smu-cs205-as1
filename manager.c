#include "manager.h"

Manager *manager_new() {
  Manager *manager = malloc(sizeof(Manager));
  *manager = (Manager) {
    .stopped = process_queue_new(),
    .running = process_queue_new(),
    .running_count = 0,
    .terminated = process_queue_new(),
  };
  return manager;
}

void manager_free(Manager *manager) {
  process_queue_free(manager->stopped);
  process_queue_free(manager->running);
  process_queue_free(manager->terminated);
  free(manager);
}

void manager_run(char **arg_list) {
}

