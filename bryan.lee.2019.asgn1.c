/**
 * STATE DIAGRAM FOR PROCESS MANAGER
 * =================================
 *
 *                  +  RUN
 *                  |
 *                  v
 *          +---------+
 *          | STOPPED +---------------------+
 *          +---------+                     |
 *            ^     |                       |
 *  STOP(PID) |     | run_available         |
 *   overflow |     | RESUME(PID)           |
 *            |     v                       |
 *          +---------+                     |
 *          | RUNNING |                     | KILL(PID)
 *          +---------+                     | EXIT
 *                  |                       |
 *                  | process_terminated    |
 *                  | KILL(PID)             |
 *                  | EXIT                  |
 *                  v                       |
 *        +-------------+                   |
 *        | TERMINATED  +<------------------+
 *        +-------------+
 *
 * structures
 * ==========
 * STOPPED - a dynamic queue implemented as a linked list.
 *         - all inserts will only ever be appended to the end of the queue.
 *
 * RUNNING - a fixed-sized queue of length = 3 implemented with an array.
 *
 * TERMINATED - a bin of processes that have been terminated.
 *            - use a simple dynamic array to store terminated values.
 *
 * events
 * ======
 * process_terminated        - an event fired by wait(int *status).
 * run_available             - an event fired after any event is popped off RUNNING.
 * RUN/STOP/RESUME/KILL/EXIT - user-generated events fired by polling for user
 *                             input with an event loop.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Maximum user input size.
#define MAX_IN 1024
// Max no. of run arguments, assuming prog length of 1 and arg length of 1:
// MAX_ARGS = (1024 - 4 [run + space] - 2 [prog + space] - 1 [\n]) // 2 = 508
#define MAX_ARGS 508
#define BASE_10 10

/**
 * @brief Get the integer value from a string representation of the pid.
 *
 * @return The integer value of pid if the format is valid. Otherwise, returns
 * -1.
 */
pid_t pid_from_str(char *str) {
  if (str == NULL) return -1;
  char *end_ptr;
  int pid = strtol(str, &end_ptr, BASE_10);
  if (*end_ptr) return -1;
  return pid;
}

char **new_arg_list_from_str(char *str) {
  int argc = 0;
  char **arg_list = calloc(MAX_ARGS, sizeof(char**));
  char *arg = strtok(str, " \n");
  while (arg != NULL) {
    arg_list[argc++] = arg;
    arg = strtok(NULL, " \n");
  };
  return arg_list;
}

void run(char *arg_list[]) {
  for (int i = 0; arg_list[i] != NULL; i++) {
    printf("%s\n", arg_list[i]);
  }
}

void stop(pid_t pid) {
  if (pid == -1) {
    printf("stopped invalid pid, try again...\n");
    return;
  }
  printf("stopping pid %d...\n", pid);
}

void kill(pid_t pid) {
  if (pid == -1) {
    printf("killed invalid pid, try again...\n");
    return;
  }
  printf("killing pid %d...\n", pid);
}

void resume(pid_t pid) {
  if (pid == -1) {
    printf("resumed invalid pid, try again...\n");
    return;
  }
  printf("resuming pid %d...\n", pid);
}

void list() {
  printf("listing processes...\n");
}

void terminate_and_exit() {
  printf("terminating all processes...\n");
}

int main() {

  char input[1024];

  // event loop to check for user input.
  while (fgets(input, MAX_IN, stdin), strcmp(input, "exit\n") != 0) {

    pid_t selected_pid;
    char *token = strtok(input, " \n");

    if (strcmp(token, "list") == 0) {
      list();
    } else if (strcmp(token, "resume") == 0) {
      token = strtok(NULL, " \n");
      selected_pid = pid_from_str(token);
      resume(selected_pid);
    } else if (strcmp(token, "kill") == 0) {
      token = strtok(NULL, " \n");
      selected_pid = pid_from_str(token);
      kill(selected_pid);
    } else if (strcmp(token, "stop") == 0) {
      token = strtok(NULL, " \n");
      selected_pid = pid_from_str(token);
      stop(selected_pid);
    } else if (strcmp(token, "run") == 0) {
      token = strtok(NULL, "\n");
      char **arg_list = new_arg_list_from_str(token);
      run(arg_list);
      free(arg_list);
    } else {
      printf("unrecognized command, try again...\n");
    }
  }

  terminate_and_exit();
}

