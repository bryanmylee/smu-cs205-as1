#include <stdio.h>
#include "process.h"

Process *process_new(pid_t pid, long last_updated, ProcessState state) {
  Process *new_process = malloc(sizeof(Process));
  *new_process = (Process) {
    .pid = pid,
    .last_updated = last_updated,
    .state = state,
  };
  return new_process;
}

ProcessQueue *process_queue_new() {
  ProcessQueue *queue = malloc(sizeof(ProcessQueue));
  *queue = (ProcessQueue) {
    .size = 0,
    .head = NULL,
    .tail = NULL,
  };
  return queue;
}

void process_queue_free(ProcessQueue *queue) {
  ProcessNode *walk = queue->head;
  ProcessNode *next;
  while (walk != NULL) {
    next = walk->next;
    free(walk->process);
    free(walk);
    walk = next;
  }
  free(queue);
}

void process_queue_enqueue(ProcessQueue *queue, Process *process) {
  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  new_node->process = process;
  if (queue->size == 0) {
    queue->head = new_node;
    queue->tail = new_node;
  } else {
    queue->tail->next = new_node;
    queue->tail = new_node;
  }
  queue->size++;
}

Process *process_queue_dequeue(ProcessQueue *queue) {
  if (queue->size == 0) return NULL;
  ProcessNode *head = queue->head;
  if (queue->size == 1) {
    queue->head = NULL;
    queue->tail = NULL;
  } else {
    queue->head = queue->head->next;
  }
  Process *res = head->process;
  free(head);
  queue->size--;
  return res;
}

Process *process_queue_remove_with_pid(ProcessQueue *queue, pid_t pid) {
  if (queue->size == 0) return NULL;
  ProcessNode *walk = queue->head;
  // process to find is at the head of the queue.
  if (walk->process->pid == pid) {
    return process_queue_dequeue(queue);
  }
  // set walk to the node before node to return.
  while (walk->next != NULL && walk->next->process->pid != pid) {
    walk = walk->next;
  }
  ProcessNode *res_node = walk->next;
  // process not found.
  if (res_node == NULL) return NULL;
  // reconnect the disjoint parts of the queue.
  walk->next = walk->next->next;
  // reassign queue tail if res_node is tail.
  if (queue->tail == res_node) {
    queue->tail = walk;
  }
  Process *res = res_node->process;
  free(res_node);
  queue->size--;
  return res;
}

void process_queue_print(ProcessQueue *queue) {
  ProcessNode *walk = queue->head;
  while (walk != NULL) {
    Process *p = walk->process;
    printf("%d,%d,%ld\n", p->pid, p->state, p->last_updated);
    walk = walk->next;
  }
}

