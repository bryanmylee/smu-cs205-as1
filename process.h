#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>
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

Process *process_new(pid_t pid, long last_updated, ProcessState state);

typedef struct process_node {
  Process *process;
  struct process_node *next;
} ProcessNode;

typedef struct process_queue {
  ProcessNode *head;
  ProcessNode *tail;
} ProcessQueue;

void process_enqueue(ProcessQueue *queue, Process *process);
Process *process_dequeue(ProcessQueue *queue);
Process *process_remove_with_pid(ProcessQueue *queue, pid_t pid);

#endif

