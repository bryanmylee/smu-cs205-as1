#include <stdbool.h>
#include <stdio.h>
#include "../process.h"

bool test_enqueue() {
  ProcessQueue queue;
  int args[3][3] = { { 0, 0, 0 }, { 1, 2, 0 }, { 2, 4, 0 } };
  for (int i = 0; i < 3; i++) {
    process_enqueue(&queue, process_new(args[i][0], args[i][1], args[i][2]));
  }
  ProcessNode *walk = queue.head;
  int i = 0;
  while (walk != NULL) {
    Process *p = walk->process;
    if (p->pid != args[i][0] || p->last_updated != args[i][1] || p->state != args[i][2]) {
      return false;
    }
    i++;
    walk = walk->next;
  }
  return true;
}

int main() {
  printf("test_enqueue: %s\n", test_enqueue() ? "passed" : "failed");
}

