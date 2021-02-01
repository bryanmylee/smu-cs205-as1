# Architecture

## Structure of state

Based on the requirements given, I knew that three structures were required,
with an overall messaging system like so:

```
      +--------------------------+-----------+
      |                          ^           |
      |                     stop |  overflow |
      |  STOPPED                 |           |
      v  ================        |           |
run +-+->|  |  | ... |  | --+    |           |
         ================   |    |           |
                            |    |           |
            |  |            |    |           |
            |  |            |                |
     resume |  |  available |   RUNNING      |
            |  |            v   ==========   |
            +---------------+-->|  |  |  | --+
               |                ==========
               |
          kill |           exits |              TERMINATED
               |                 v              ================
               +-----------------+------------->|  |  | ... |  |
                                                ================
```

As such, the main structure for the entire manager process revolves around the
`Manager` struct:

```c
typedef struct manager {
  ProcessQueue stopped;
  ProcessQueue running;
  ProcessQueue terminated;
} Manager;
```

`ProcessQueue` is a custom data structure backed by a linked list containing
`Process` elements.

Each `Process` has the following structure:

```c
typedef struct process {
  pid_t pid;
  long last_updated;
  ProcessState state;
} Process;
```

For more information about the data structures used, see `process.h`.

## Event-driven architecture

Due to the number of messages being passed around, I decided that the best
solution for the process manager is an event-driven architecture.

I wanted to avoid using a complicated third-party event-driven library.
To design my own, I had to answer several questions:

1. Where do events come from?
2. How do we listen for and handle events?

### Sources of events

Events are generated from two sources:

1. User input via `stdin` -- `run`, `stop`, `resume`, `kill`, `exit`.
2. Terminating processes triggering `waitpid`.

### Listening for events

By far the biggest challenge was in figuring out how to listen to both sources
of events simultaneously. Reading from `stdin` is a blocking operation (`scanf`,
`readline`, `fgets`). However, we want to continuously poll for state change in
child processes with `waitpid`. As such, we need to run two concurrent event
loops.

The initial idea was to run the user input listener as a child process of the
main process. However, this causes a divergence in the memory space used by the
main and listener process.

After much experimentation, I determined that the best solution was to use a
non-blocking pipe between the main and child process.

The child process writes to the pipe when it receives a line of user input.

The main process actively polls the pipe, and triggers user events when the pipe
is non-empty.

## Architectural Diagram

The final design for the event-driven architecture is as follows:

```
          INPUT_LISTENER
+----------------------+
|            +------+  |
|            |      |  |
|            v      |  |
|  +------------+   |  |
|  | read input +---+  |
|  +------------+      |
+--------||------------+
         ||
         || input_pipe
         ||                        MAIN_PROCESS
+--------||-----------------------------------+
|  +--------------+       +----------------+  |
|  | handle_input +------>| poll_processes |  |
|  +--------------+       +-------+--------+  |
|          ^                      |           |
|          |                      |           |
|  +-------+---------+            |           |
|  | reconcile_state |<-----------+           |
|  +-----------------+                        |
+---------------------------------------------+
```

### `INPUT_LISTENER`

`INPUT_LISTENER` is a child process that listens for user input via `stdin`,
then writes it into `input_pipe`.

If `exit` is received, then the process will close the `input_pipe` and
terminate.

### `MAIN_PROCESS`

`MAIN_PROCESS` runs an event loop that cycles through three phases:

1. `handle_input`
2. `poll_processes`
3. `reconcile_state`

#### `handle_input`

In this phase, the main process checks the input pipe for any data. If an input
exists, it triggers user-driven events as defined in
[REQUIREMENTS.md](REQUIREMENTS.md#commands).

#### `poll_processes`

During the polling phase, the main process checks the running child processes
for any termination event with `waitpid`. If a child terminates, the manager
updates its state to reflect the change and fires the termination event.

#### `reconcile_state`

It is important to note that the previous two phases will only write to the
manager state without making any guarantees of validity. Therefore, the manager
may be left in an invalid state:

1. More than 3 running processes.
2. Pending stopped processes while there are fewer than 3 running processes.

> This is both for performance and code maintenance reasons.

Rather than revalidating state for every event triggered, we can simplify the
code by reconciling the state at the end of each event loop iteration.

This helps us manage our code complexity, as all reconciliation logic is
neatly kept in one location.

Furthermore, it prevents mistakes from occurring should we forget to clean up
the state after any event handling.

