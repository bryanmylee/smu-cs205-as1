#ifndef MANAGER_H
#define MANAGER_H

#include <stdbool.h>
#include <time.h>
#include "process.h"

typedef struct manager {
    ProcessQueue *stopped;
    ProcessQueue *running;
    ProcessQueue *terminated;
} Manager;

/**
 * @brief Initialize a new allocated Manager.
 *
 * @return The allocated Manager.
 */
Manager *manager_new();

/**
 * @brief Clean up the internal structures of a Manager and free.
 *
 * All queues within the manager will also be freed.
 *
 * @param manager The Manager to free.
 */
void manager_free(Manager *manager);

/**
 * @brief Execute a child process with specific arguments.
 *
 * Child processes which are executed through this function will be managed.
 *
 * @param manager  The Manager instance.
 * @param arg_list An argument list of strings to be passed to execvp. Similar
 *                 to execvp, the final argument must be NULL.
 *
 * @return true if successfully forked and executed, false otherwise.
 */
bool manager_run(Manager *manager, char **arg_list);

/**
 * @brief Suspend the execution of a running child process with a given pid.
 *
 * @param manager The Manager instance.
 * @param pid     The pid of the running child process to stop.
 *
 * @return true if successfully suspended, false if the process is not currently
 *         running or found.
 */
bool manager_stop(Manager *manager, pid_t pid);

/**
 * @brief Resume the execution of a stopped child process with a given pid.
 *
 * If the running queue is full, the earliest running process will be suspended.
 *
 * @param manager The Manager instance.
 * @param pid     The pid of the suspended child process to run.
 *
 * @return true if successfully resumed, false if the process is not currently
 *         stopped or found.
 */
bool manager_force_resume(Manager *manager, pid_t pid);

/**
 * @brief Kill a running or suspended child process with a given pid.
 *
 * @param manager The Manager instance.
 * @param pid     The pid of the process to kill.
 *
 * @return true if successfully killed, false if the process is not found.
 */
bool manager_terminate(Manager *manager, pid_t pid);

/**
 * @brief Kill all running and/or suspended child processes.
 *
 * @param manager The Manager instance.
 */
void manager_terminate_all(Manager *manager);

/**
 * @brief Print all child processes to stdout delimited by line breaks.
 *
 * @param manager The Manager instance.
 */
void manager_list(Manager *manager);

/**
 * @brief Check all child processes for a termination event, and adjust the
 *        state of the Manager accordingly.
 *
 * If a child process is found to have terminated, then its Process instance
 * will be moved to the terminated queue and have its properties updated.
 *
 * Internally, this function uses a non-blocking waitpid.
 *
 * @param manager The Manager instance.
 */
void manager_poll_processes(Manager *manager);

/**
 * @brief Adjust the state of the Manager such that it fits within the
 *        constraints defined.
 *
 * More specifically, this ensures that the running queue is always as full as
 * possible.
 * - If there are empty spaces in the running queue, the earliest available
 *   suspended processes will be resumed.
 * - If there are more than 3 running processes, the earliest running processes
 *   will be suspended.
 *
 * @param manager The Manager instance.
 */
void manager_reconcile_state(Manager *manager);

#endif

