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

/**
 * @brief Initialize a new allocated Process.
 *
 * Make sure to free the Process after usage.
 *
 * @param pid          The pid of the child process being tracked.
 * @param last_updated Last time since the process state was updated in Epoch
 *                     seconds.
 * @param state        The state of the process.
 *
 * @return The allocated Process.
 */
Process *process_new(pid_t pid, long last_updated, ProcessState state);

typedef struct process_node {
  Process *process;
  struct process_node *next;
} ProcessNode;

typedef struct process_queue {
  int size;
  ProcessNode *head;
  ProcessNode *tail;
} ProcessQueue;

/**
 * @brief Initialize a new allocated ProcessQueue.
 *
 * Make sure to free the ProcessQueue after usage.
 * - See process_queue_free(ProcessQueue *queue).
 *
 * @return The allocated ProcessQueue.
 */
ProcessQueue *process_queue_new(void);

/**
 * @brief Clean up the internal structures of a ProcessQueue and free.
 *
 * All nodes and processes within the queue will also be freed.
 *
 * @param queue The ProcessQueue to free.
 */
void process_queue_free(ProcessQueue *queue);

/**
 * @brief Enqueue a Process into a ProcessQueue.
 *
 * Make sure to allocate the process to heap before enqueuing.
 *
 * @param queue   The receiving queue.
 * @param process The allocated process to enqueue.
 */
void process_queue_enqueue(ProcessQueue *queue, Process *process);

/**
 * @brief Dequeue a Process from the head of a ProcessQueue.
 *
 * @param queue The queue to retrieve from.
 *
 * @return The dequeued process.
 */
Process *process_queue_dequeue(ProcessQueue *queue);

/**
 * @brief Remove a Process from a ProcessQueue by its pid.
 *
 * @param queue The queue to search.
 * @param pid   The pid of the Process to find and remove.
 *
 * @return The removed process.
 */
Process *process_queue_remove_with_pid(ProcessQueue *queue, pid_t pid);

/**
 * @brief Print a ProcessQueue to stdout delimited by line breaks.
 *
 * Each Process will be printed in the format: pid,state,last_updated.
 *
 * @param queue The queue to print.
 */
void process_queue_print(ProcessQueue *queue);

#endif

