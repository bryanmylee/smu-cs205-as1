#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Get the integer value from a string representation of the pid.
 *
 * @return The value of pid if the format is valid, otherwise returns -1.
 */
pid_t pid_from_str(char *str);

/**
 * @brief Generate a list of arguments from a space-delimited string.
 *
 * Note that the argument list uses the same memory space allocated for the
 * original string.
 *
 * Therefore, the original string is modified.
 *
 * @param str      The space-delimited argument string.
 * @param max_args The maximum number of arguments to be passed to the
 *                 program.
 *
 * @return A NULL-terminated, allocated list of arguments with MAX_ARGS
 *         elements.
 */
char **new_arg_list_from_str(char *str, int max_args);

#ifdef DEV
#define dev_printf(format, ...) printf(format, ##__VA_ARGS__)
#else
#define dev_printf(format, ...)
#endif

#endif

