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

void process_enqueue(ProcessQueue *queue, Process *process) {
  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  new_node->process = process;
  // head is also NULL.
  if (queue->tail == NULL) {
    queue->head = new_node;
    queue->tail = new_node;
  } else {
    queue->tail->next = new_node;
    queue->tail = new_node;
  }
}

Process *process_dequeue(ProcessQueue *queue) {
  ProcessNode *head = queue->head;
  // tail is also NULL -> 0 elements.
  if (head == NULL) {
    return NULL;
  // 1 element.
  } else if (head == queue->tail) {
    queue->head = NULL;
    queue->tail = NULL;
  // 2 or more elements.
  } else {
    queue->head = queue->head->next;
  }
  Process *res = head->process;
  free(head);
  return res;
}

Process *process_remove_with_pid(ProcessQueue *queue, pid_t pid) {
  ProcessNode *walk = queue->head;
  ProcessNode *res_node = NULL;
  // process to find is at the head of the queue.
  if (walk == NULL) return NULL;
  if (walk->process->pid == pid) {
    return process_dequeue(queue);
  }
  // set walk to the node before node to return.
  while (walk->next != NULL && walk->next->process->pid != pid) {
    res_node = walk->next;
    // reconnect the disjoint parts of the queue.
    walk->next = walk->next->next;
  }
  res_node = walk->next;

  // not found.
  if (res_node == NULL) return NULL;
  Process *res = res_node->process;
  free(res_node);
  return res;
}

