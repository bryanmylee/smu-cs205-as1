#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include "manager.h"
#include "utils.h"

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

bool manager_run(Manager *manager, char **arg_list) {
  pid_t pid = fork();
  if (pid > 0) {
    dev_printf("manager created child with pid %d\n", pid);
    Process *new_process = process_new(pid, time(0), STOPPED);
    process_queue_enqueue(manager->stopped, new_process);
    kill(pid, SIGSTOP);
    return true;
  }
  if (pid == 0) {
    fprintf(stderr, "fork failed\n");
    if (arg_list[0] == NULL) return false;
    dev_printf("child created with arg_list: ");
    for (int i = 0; arg_list[i] != NULL; i++) {
      dev_printf("%s ", arg_list[i]);
    }
    dev_printf("\n");
    execvp(arg_list[0], arg_list);
    return true;
  }
  fprintf(stderr, "fork failed\n");
  return false;
}

bool manager_stop_process(Manager *manager, Process *to_stop) {
  if (to_stop == NULL) return false;
  kill(to_stop->pid, SIGSTOP);
  to_stop->last_updated = time(0);
  to_stop->state = STOPPED;
  process_queue_enqueue(manager->stopped, to_stop);
  return true;
}

bool manager_stop(Manager *manager, pid_t pid) {
  Process *to_stop = process_queue_remove_with_pid(manager->running, pid);
  return manager_stop_process(manager, to_stop);
}

bool manager_force_resume(Manager *manager, pid_t pid) {
  Process *to_resume = process_queue_remove_with_pid(manager->stopped, pid);
  if (to_resume == NULL) return false;
  kill(to_resume->pid, SIGCONT);
  to_resume->last_updated = time(0);
  to_resume->state = RUNNING;
  process_queue_enqueue(manager->running, to_resume);
  return true;
}

bool manager_terminate_process(Manager *manager, Process *to_terminate) {
  if (to_terminate == NULL) return false;
  kill(to_terminate->pid, SIGKILL);
  to_terminate->last_updated = time(0);
  to_terminate->state = TERMINATED;
  process_queue_enqueue(manager->terminated, to_terminate);
  return true;
}

bool manager_terminate_running(Manager *manager, pid_t pid) {
  Process *to_terminate = process_queue_remove_with_pid(manager->running, pid);
  return manager_terminate_process(manager, to_terminate);
}

bool manager_terminate_stopped(Manager *manager, pid_t pid) {
  Process *to_terminate = process_queue_remove_with_pid(manager->stopped, pid);
  return manager_terminate_process(manager, to_terminate);
}

bool manager_terminate(Manager *manager, pid_t pid) {
  dev_printf("manager terminating pid %d...\n", pid);
  return manager_terminate_running(manager, pid)
    || manager_terminate_stopped(manager, pid);
}

void manager_terminate_all(Manager *manager) {
  Process *to_terminate;
  while ((to_terminate = process_queue_dequeue(manager->running)) != NULL) {
    manager_terminate_process(manager, to_terminate);
  }
  while ((to_terminate = process_queue_dequeue(manager->stopped)) != NULL) {
    manager_terminate_process(manager, to_terminate);
  }
}

void manager_list(Manager *manager) {
  dev_printf("RUNNING\n");
  process_queue_print(manager->running);
  dev_printf("STOPPED\n");
  process_queue_print(manager->stopped);
  dev_printf("TERMINATED\n");
  process_queue_print(manager->terminated);
}

void manager_poll_processes(Manager *manager) {
  int status;
  pid_t child_pid = waitpid(-1, &status, WNOHANG);
  if (child_pid > 0) {
    dev_printf("pid %d exited with code %d\n", child_pid, status);
    manager_terminate(manager, child_pid);
  }
}

bool manager_handle_run_available(Manager *manager) {
  dev_printf("run available...\n");
  Process *to_run = process_queue_dequeue(manager->stopped);
  if (to_run == NULL) return false;
  kill(to_run->pid, SIGCONT);
  to_run->last_updated = time(0);
  to_run->state = RUNNING;
  process_queue_enqueue(manager->running, to_run);
  return true;
}

void manager_reconcile_state(Manager *manager) {
  // pending stopped events and available running space.
  while (manager->stopped->size > 0 && manager->running->size < MAX_RUN) {
    manager_handle_run_available(manager);
  }
  // overflow of running.
  while (manager->running->size > MAX_RUN) {
    Process *earliest = process_queue_dequeue(manager->running);
    manager_stop_process(manager, earliest);
  }
}

