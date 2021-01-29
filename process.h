#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>

typedef enum process_state {
  RUNNING = 0,
  STOPPED = 1,
  TERMINATED = 2,
} ProcessState;

typedef struct process {
  pid_t pid;
  long last_updated;
  ProcessState state;
} Process;

typedef struct process_node {
  Process process;
  struct process_node *next;
} ProcessNode;

typedef struct process_queue {
  ProcessNode head;
  ProcessNode tail;
} ProcessQueue;

#endif

