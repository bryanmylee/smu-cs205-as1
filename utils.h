#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pid_t pid_from_str(char *str);
char **new_arg_list_from_str(char *str, int max_args);
void dev_printf(const char *fmt, ...);

#endif

