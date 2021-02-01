#include <fcntl.h>
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

void process_stop(Manager *manager, pid_t pid) {
  if (pid == -1) {
    printf("stopped invalid pid, try again...\n");
    return;
  }
  printf("stopping %d...\n", pid);
  if (manager_stop(manager, pid)) {
    printf("stopped %d successfully.\n", pid);
  } else {
    printf("stop failed, %d not found.\n", pid);
  }
}

void process_kill(Manager *manager, pid_t pid) {
  if (pid == -1) {
    printf("killed invalid pid, try again...\n");
    return;
  }
  printf("killing %d...\n", pid);
  if (manager_terminate(manager, pid)) {
    printf("killed %d successfully.\n", pid);
  } else {
    printf("kill failed, %d not found.\n", pid);
  }
}

void process_resume(Manager *manager, pid_t pid) {
  if (pid == -1) {
    printf("resumed invalid pid, try again...\n");
    return;
  }
  printf("resuming %d...\n", pid);
  if (manager_force_resume(manager, pid)) {
    printf("resumed %d successfully.\n", pid);
  } else {
    printf("resume failed, %d not found.\n", pid);
  }
}

void process_list(Manager *manager) {
  dev_printf("listing processes...\n");
  manager_list(manager);
}

void process_terminate_all(Manager *manager) {
  dev_printf("terminating all processes...\n");
  manager_terminate_all(manager);
}

/**
 * @brief A child process that pipes stdin into a data link.
 *
 * @param link[] The data link to write into.
 */
void run_input_listener(int link[]) {
  // close the head of the link before writing.
  close(link[0]);

  char input[MAX_IN];
  // read input from stdin (blocking).
  while (fgets(input, MAX_IN, stdin), strcmp(input, "exit\n") != 0) {
    // write input into the tail of the link.
    write(link[1], input, MAX_IN);
  }
}

void handle_input(Manager *manager, char *input) {
  pid_t selected_pid;
  char *token = strtok(input, " \n");
  if (strcmp(token, "list") == 0) {
    process_list(manager);
  } else if (strcmp(token, "resume") == 0) {
    token = strtok(NULL, " \n");
    selected_pid = pid_from_str(token);
    process_resume(manager, selected_pid);
  } else if (strcmp(token, "kill") == 0) {
    token = strtok(NULL, " \n");
    selected_pid = pid_from_str(token);
    process_kill(manager, selected_pid);
  } else if (strcmp(token, "stop") == 0) {
    token = strtok(NULL, " \n");
    selected_pid = pid_from_str(token);
    process_stop(manager, selected_pid);
  } else if (strcmp(token, "run") == 0) {
    token = strtok(NULL, "\n");
    char **arg_list = new_arg_list_from_str(token, MAX_ARGS);
    run(manager, arg_list);
    free(arg_list);
  } else {
    printf("unrecognized command, try again...\n");
  }
}

/**
 * @brief The parent process that asynchronously reads from a data link.
 *
 * @param link[] The data link to read from.
 */
void run_parent_event_loop(Manager *manager, int link[]) {
  // close the tail of the link before reading.
  close(link[1]);

  char input[MAX_IN];
  int nread;
  // read from the head of the link until EOF.
  while ((nread = read(link[0], input, MAX_IN)) != 0) {
    // input link is not empty.
    if (nread != -1) {
      handle_input(manager, input);
    }
    manager_poll_processes(manager);
    manager_reconcile_state(manager);
  }
  close(link[0]);
  process_terminate_all(manager);
}

int main() {
  Manager *manager = manager_new();

  // Set up a non-blocking data link between manager and user input process.
  // read from the head at link[0], and write to the tail at link[1].
  int link[2];
  if (pipe(link) < 0) {
    exit(1);
  }
  if (fcntl(link[0], F_SETFL, O_NONBLOCK) < 0) {
    exit(2);
  }

  // listen for user input and trigger user events.
  pid_t input_pid = fork();
  if (input_pid > 0) {
    run_parent_event_loop(manager, link);
  } else if (input_pid == 0) {
    run_input_listener(link);
  } else {
    fprintf(stderr, "fork failed\n");
  }
}

