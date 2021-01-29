<h1 align="center">
  CS205 Operating System Concepts with Android
</h1>
<h2 align="center">
  Assignment 1 – Process Management
</h2>

### Requirements

Write a process manager that allows user to run at most three processes at the same time in the background. In other words, there are at most three processes in the **"running"** state. Further running a new process will put the process in the **"stopped"** state. The process will be put in the running state automatically when the number of running processes is fewer than three. Whenever a process is completed it will be in the **"terminated**" state and cannot be resumed to either running or stopped state. You should design appropriate data structure(s) to manage the processes of different states.

The scheduling of processes follows **FIFO** (first-in-first-out). For example, if there are five processes in the stopped state, the process that will be scheduled for running next is the process that waits for the longest in the stopped state. Similarly, when an existing process is being stopped, it can be in the running state again only after all the current processes in the stopped state have entered the running state. A newly created process can always enter the running state if no process is in the stopped state.

Using your program, a user can interactively list, run, stop, resume or terminate a process, using the following commands:

* `run [program] [arguments]`: Running an executable program with optional input arguments
* `stop [PID]`: Put the running process with the specified PID in the stopped state, and then dispatch the longest-waiting process in the running state
* `kill [PID]`: Terminate the process with the specified PID. If the process is currently running, dispatch the longest-waiting process in the running state
* `resume [PID]`: Resume the stopped process with the specified PID in the running state. If the number of running processes is going to exceed three, then the current longest-running process will have to be put in the stopped state
* `list` : List all the process by: PID, states, and the time at which state has last changed
* `exit` : Terminate all child processes if they have not yet been terminated, and then exit
from parent program

The executable file we will use for testing is prog which takes two arguments: a *file name*, a *number n*. 

`prog` would write a statement to the file every second for n seconds.

Sample runs of `prog`:

```bash
$ ./prog a.txt 2
$ cat a.txt
Process ran 2 out of 2 secs
```

```bash
$./prog b.txt 5
^C
$ cat b.txt
Process ran 4 out of 5 secs
```
