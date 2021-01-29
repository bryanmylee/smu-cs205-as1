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

