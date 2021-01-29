#include <stdbool.h>
#include <stdio.h>
#include "../process.h"

bool process_equal(Process *p, pid_t pid, long last_updated, ProcessState state) {
  return p->pid == pid && p->last_updated == last_updated && p->state == state;
}

bool test_enqueue() {
  ProcessQueue queue = { .head = NULL, .tail = NULL };
  int args[3][3] = { { 0, 0, 0 }, { 1, 2, 0 }, { 2, 4, 0 } };
  for (int i = 0; i < 3; i++) {
    process_enqueue(&queue, process_new(args[i][0], args[i][1], args[i][2]));
  }
  ProcessNode *walk = queue.head;
  int i = 0;
  while (walk != NULL) {
    Process *p = walk->process;
    if (!process_equal(p, args[i][0], args[i][1], args[i][2])) {
      return false;
    }
    i++;
    walk = walk->next;
  }
  return true;
}

bool test_dequeue() {
  ProcessQueue queue = { .head = NULL, .tail = NULL };
  int args[3][3] = { { 0, 0, 0 }, { 1, 2, 0 }, { 2, 4, 0 } };
  for (int i = 0; i < 3; i++) {
    process_enqueue(&queue, process_new(args[i][0], args[i][1], args[i][2]));
  }
  Process *p = process_dequeue(&queue);
  int i = 0;
  while (p != NULL) {
    if (!process_equal(p, args[i][0], args[i][1], args[i][2])) {
      return false;
    }
    i++;
    p = process_dequeue(&queue);
  }
  return true;
}

bool test_remove_with_pid() {
  ProcessQueue queue = { .head = NULL, .tail = NULL };
  int args[3][3] = { { 0, 0, 0 }, { 1, 2, 0 }, { 2, 4, 0 } };
  for (int i = 0; i < 3; i++) {
    process_enqueue(&queue, process_new(args[i][0], args[i][1], args[i][2]));
  }
  int indices[] = { 1, 2, 0 };
  for (int i = 0; i < 3; i++) {
    int j = indices[i];
    Process *p = process_remove_with_pid(&queue, j);
    if (!process_equal(p, args[j][0], args[j][1], args[j][2])) {
      return false;
    }
  }
  return true;
}

bool test_remove_with_pid_head_first() {
  ProcessQueue queue = { .head = NULL, .tail = NULL };
  int args[3][3] = { { 0, 0, 0 }, { 1, 2, 0 }, { 2, 4, 0 } };
  for (int i = 0; i < 3; i++) {
    process_enqueue(&queue, process_new(args[i][0], args[i][1], args[i][2]));
  }
  int indices[] = { 0, 1, 2 };
  for (int i = 0; i < 3; i++) {
    int j = indices[i];
    Process *p = process_remove_with_pid(&queue, j);
    if (!process_equal(p, args[j][0], args[j][1], args[j][2])) {
      return false;
    }
  }
  return true;
}

int main() {
  printf("test_enqueue: %s\n", test_enqueue() ? "passed" : "failed");
  printf("test_dequeue: %s\n", test_dequeue() ? "passed" : "failed");
  printf("test_remove_with_pid %s\n", test_remove_with_pid() ? "passed" : "failed");
  printf("test_remove_with_pid_head_first %s\n", test_remove_with_pid_head_first() ? "passed" : "failed");
}

