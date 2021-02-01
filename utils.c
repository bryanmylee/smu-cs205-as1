#include <stdarg.h>
#include <stdio.h>
#include "utils.h"

#define BASE_10 10

/**
 * @brief Get the integer value from a string representation of the pid.
 *
 * @return The integer value of pid if the format is valid. Otherwise, returns
 *         -1.
 */
pid_t pid_from_str(char *str) {
  if (str == NULL) return -1;
  char *end_ptr;
  int pid = strtol(str, &end_ptr, BASE_10);
  if (*end_ptr) return -1;
  return pid;
}

/**
 * @brief Generate a list of arguments from a space-delimited string.
 *
 * Note that the argument list uses the same memory space allocated for the
 * original string.
 *
 * Therefore, the original string is modified.
 *
 * @param str The space-delimited argument string.
 *
 * @return A NULL-terminated, allocated list of arguments with MAX_ARGS
 *         elements.
 */
char **new_arg_list_from_str(char *str, int max_args) {
  int argc = 0;
  char **arg_list = calloc(max_args, sizeof(char**));
  char *arg = strtok(str, " \n");
  while (arg != NULL) {
    arg_list[argc++] = arg;
    arg = strtok(NULL, " \n");
  };
  return arg_list;
}

#define DEV 0
void dev_printf(const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  if (DEV) {
    vprintf(fmt, arg);
  }
  va_end(arg);
}

