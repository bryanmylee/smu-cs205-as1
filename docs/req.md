# Requirements

Write a process manager that allows user to run at most three processes at the same time in the background. In other words, there are at most three processes in the **"running"** state. Further running a new process will put the process in the **"stopped"** state. The process will be put in the running state automatically when the number of running processes is fewer than three. Whenever a process is completed it will be in the **"terminated**" state and cannot be resumed to either running or stopped state. You should design appropriate data structure(s) to manage the processes of different states.

The scheduling of processes follows **FIFO** (first-in-first-out). For example, if there are five processes in the stopped state, the process that will be scheduled for running next is the process that waits for the longest in the stopped state. Similarly, when an existing process is being stopped, it can be in the running state again only after all the current processes in the stopped state have entered the running state. A newly created process can always enter the running state if no process is in the stopped state.

## Commands

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

## Sample runs of `prog`

```bash
$ ./prog a.txt 2
$ cat a.txt
Process ran 2 out of 2 secs
```

```bash
$ ./prog b.txt 5
^C
$ cat b.txt
Process ran 4 out of 5 secs
```

## Sample runs of process manager

**Example 1**: admit five processes but put the fourth and fifth one in stopped state. In the background, as one of the processes terminates, we automatically resume the process that has waited the longest. Similarly killing a process would also resume the process that has waited the longest.

```bash
$ ./a.out
$ run prog x40 40
$ run prog x50 50
$ run prog x60 60
$ run prog x70 70
$ run prog x80 80
$ list
1697140,0,1611329874
1697145,0,1611329880
1697146,0,1611329884
1697149,1,1611329888
1697154,1,1611329899
$ list
1697140,2,1611329914
1697145,0,1611329880
1697146,0,1611329884
1697149,0,1611329914
1697154,1,1611329899
$ kill 1697145
terminating 1697145
$ list
1697140,2,1611329914
1697145,2,1611329922
1697146,0,1611329884
1697149,0,1611329914
1697154,0,1611329922
```

**Example 2**: admit five processes but put the fourth and fifth one in stopped state. Stopping a process would result in the running of the process that has waited the longest. Resuming a process would put the longest running process to stopped state.

```bash
$ ./a.out
$ run prog x40 40
$ run prog x50 50
$ run prog x60 60
$ run prog x70 70
$ run prog x80 80
$ list
1697479,0,1611330691
1697480,0,1611330694
1697483,0,1611330699
1697486,1,1611330702
1697489,1,1611330706
$ stop 1697480
stopping 1697480
$ list
1697479,2,1611330731
1697480,1,1611330729
1697483,0,1611330699
1697486,0,1611330729
1697489,0,1611330731
$ resume 1697480
resuming 1697480
$ list
1697479,2,1611330731
1697480,0,1611330749
1697483,1,1611330749
1697486,0,1611330729
1697489,0,1611330731
```

