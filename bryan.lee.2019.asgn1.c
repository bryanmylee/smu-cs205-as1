#include <stdio.h>
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
  // By specification, the first argument contains only the program name.
  // We need to prepend the program name with "./" to find the program
  // relatively.
  // Memory has already been allocated for the characters before "run ":
  // "run prog..." --> "ru./prog..."
  //      ^               ^
  //      arg_list[0]     arg_list[0]
  arg_list[0] -= 2;
  arg_list[0][0] = '.';
  arg_list[0][1] = '/';
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

void list(Manager *manager) {
  printf("listing processes...\n");
  manager_list(manager);
}

void terminate_all() {
  printf("terminating all processes...\n");
}

int main() {
  Manager *manager = manager_new();
  char input[MAX_IN];
  // listen for user input and trigger user events.
  while (fgets(input, MAX_IN, stdin), strcmp(input, "exit\n") != 0) {
    pid_t selected_pid;
    char *token = strtok(input, " \n");
    if (strcmp(token, "list") == 0) {
      list(manager);
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
      char **arg_list = new_arg_list_from_str(token, MAX_ARGS);
      run(manager, arg_list);
      free(arg_list);
    } else {
      printf("unrecognized command, try again...\n");
    }
  }
  terminate_all();
}

