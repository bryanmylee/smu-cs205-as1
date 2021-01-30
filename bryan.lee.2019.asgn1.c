/**
 * STATE DIAGRAM FOR PROCESS MANAGER
 * =================================
 *
 *                    +
 *                    |
 *                    +---+
 *                    |   | listen_input (child)
 *                    |<--+
 *                    |
 *                    | RUN
 *                    v
 *            +---------+
 *            | STOPPED +---------------+
 *            +---------+               |
 *              ^     |                 |
 *    STOP(PID) |     | run_available   |
 *     overflow |     | RESUME(PID)     |
 *              |     v                 |
 *            +---------+               |
 *            | RUNNING |               | KILL(PID)
 *            +---------+               | EXIT
 *                    |                 |
 *                +-->+                 |
 *   process_term |   |                 |
 *                +---+ EXIT            |
 *                    | KILL(PID)       |
 *                    v                 |
 *          +-------------+             |
 *          | TERMINATED  +<------------+
 *          +-------------+
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
 * process_term       - an event loop on the parent process checking for
 *                      wait(int *status).
 * listen_input       - an event loop on a child process to handle user input.
 * run_available      - an event fired after any event is popped off RUNNING.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "manager.h"
#include "process.h"
#include "utils.h"

// Maximum user input size.
#define MAX_IN 1024
// Max no. of run arguments, assuming prog length of 1 and arg length of 1:
// MAX_ARGS = (1024 - 4 [run + space] - 2 [prog + space] - 1 [\n]) // 2 = 508
#define MAX_ARGS 508

void run(Manager *manager, char *arg_list[]) {
  manager_run(manager, arg_list);
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

void terminate_all() {
  printf("terminating all processes...\n");
}

/**
 * @brief Run one iteration of the user-driven event loop.
 *
 * @param manager The current manager instance.
 * @param input   The user input for this iteration.
 */
void run_input_event_loop(Manager *manager, char *input) {
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
    // A slice of input after "run ".
    token = strtok(NULL, "\n");
    char **arg_list = new_arg_list_from_str(token, MAX_ARGS);
    // By specification, the first argument contains only the program name.
    // We need to prepend the program name with "./" to find the program
    // relatively.
    // Memory has already been allocated for the characters in "run ".
    // "run prog..." --> "ru./prog..."
    //      ^               ^
    //      arg_list[0]     arg_list[0]
    arg_list[0] -= 2;
    arg_list[0][0] = '.';
    arg_list[0][1] = '/';
    run(manager, arg_list);
    free(arg_list);
  } else {
    printf("unrecognized command, try again...\n");
  }
}

int main() {

  Manager *manager = manager_new();
  pid_t input_event_loop_pid = fork();

  if (input_event_loop_pid > 0) {
    int status;
    while (waitpid(input_event_loop_pid, &status, WNOHANG) <= 0);
    printf("input event loop exited with status %d\n", status);
    terminate_all();
    manager_free(manager);
  } else if (input_event_loop_pid == 0) {
    char input[1024];
    // event loop to check for user input and fire user events.
    while (fgets(input, MAX_IN, stdin), strcmp(input, "exit\n") != 0) {
      run_input_event_loop(manager, input);
    }
  } else {
    fprintf(stderr, "failed to start input event loop\n");
  }
}

