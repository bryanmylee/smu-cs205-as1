#include <stdio.h>
#include "manager.h"

Manager *manager_new() {
  Manager *manager = malloc(sizeof(Manager));
  *manager = (Manager) {
    .stopped = process_queue_new(),
    .running = process_queue_new(),
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

void manager_run(Manager *manager, char **arg_list) {
  pid_t pid = fork();
  if (pid > 0) {
    printf("manager created child with pid %d\n", pid);
    Process *new_process = process_new(pid, time(0), RUNNING);
    process_queue_enqueue(manager->running, new_process);
  } else if (pid == 0) {
    if (arg_list[0] == NULL) return;
    printf("child created with arg_list: ");
    for (int i = 0; arg_list[i] != NULL; i++) {
      printf("%s ", arg_list[i]);
    }
    printf("\n");
    execvp(arg_list[0], arg_list);
  } else {
    fprintf(stderr, "fork failed\n");
  }
}

bool manager_handle_process_exit(Manager *manager, pid_t pid) {
  printf("manager terminating pid %d...\n", pid);
  Process *terminated = process_queue_remove_with_pid(manager->running, pid);
  if (terminated == NULL) return false;
  terminated->last_updated = time(0);
  terminated->state = TERMINATED;
  process_queue_enqueue(manager->terminated, terminated);
  return true;
}

void manager_list(Manager *manager) {
  process_queue_print(manager->running);
  process_queue_print(manager->stopped);
  process_queue_print(manager->terminated);
}

