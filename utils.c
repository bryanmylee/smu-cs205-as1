#include <stdarg.h>
#include <stdio.h>
#include "utils.h"

#define BASE_10 10

pid_t pid_from_str(char *str) {
  if (str == NULL) return -1;
  char *end_ptr;
  int pid = strtol(str, &end_ptr, BASE_10);
  if (*end_ptr) return -1;
  return pid;
}

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

