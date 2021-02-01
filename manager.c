#include <stdio.h>
#include <signal.h>
#include "manager.h"

#define MAX_RUN 3

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
    Process *new_process = process_new(pid, time(0), STOPPED);
    process_queue_enqueue(manager->stopped, new_process);
    kill(pid, SIGSTOP);
    if (manager->running->size < MAX_RUN) {
      manager_handle_run_available(manager);
    }
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

void manager_poll_processes(Manager *manager) {
  int status;
  pid_t child_pid = waitpid(-1, &status, WNOHANG);
  if (child_pid > 0) {
    printf("pid %d exited with code %d\n", child_pid, status);
    manager_handle_process_exit(manager, child_pid);
  }
}

bool manager_handle_run_available(Manager *manager) {
  if (manager->running->size >= MAX_RUN) return false;
  printf("run available...\n");
  Process *to_run = process_queue_dequeue(manager->stopped);
  if (to_run == NULL) return false;
  kill(to_run->pid, SIGCONT);
  to_run->last_updated = time(0);
  to_run->state = RUNNING;
  process_queue_enqueue(manager->running, to_run);
  return true;
}

bool manager_handle_process_exit(Manager *manager, pid_t pid) {
  printf("manager terminating pid %d...\n", pid);
  Process *terminated = process_queue_remove_with_pid(manager->running, pid);
  if (terminated == NULL) return false;
  terminated->last_updated = time(0);
  terminated->state = TERMINATED;
  process_queue_enqueue(manager->terminated, terminated);
  return manager_handle_run_available(manager);
}

void manager_list(Manager *manager) {
  printf("running\n");
  process_queue_print(manager->running);
  printf("\nstopped\n");
  process_queue_print(manager->stopped);
  printf("\nterminated\n");
  process_queue_print(manager->terminated);
}

