#include "process.h"

void process_enqueue(ProcessQueue *queue, Process *process) {
  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  queue->tail->next = new_node;
  queue->tail = new_node;
}

Process *process_dequeue(ProcessQueue *queue) {
  ProcessNode *head = queue->head;
  queue->head = queue->head->next;
  Process *res = head->process;
  free(head);
  return res;
}

