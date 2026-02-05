#pragma once

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flag for SIGINT signal (set to 1 when signal is received)
 */
extern int sigint_flag;

/**
 * @brief Signal handler for SIGINT (sets flag to 1)
 */
void signal_handler(int signum);

/**
 * @brief Reads drive commands from `in_fd` and sends them to the robot using `out_fd`
 *
 * @param in_fd File descriptor to read commands
 */
void relay_drive_commands(int in_fd);

/**
 * @brief Reads a line from a file descriptor
 *
 * @param fd File descriptor to read from
 * @param buffer Buffer to store the line
 * @param size Size of the buffer
 */
ssize_t read_line(int fd, char *buffer, size_t size);

/**
 * @brief Parses commands from the input file descriptor and sends them to the output file descriptor
 *
 * @param in_fd  File descriptor to read commands from
 * @param out_fd File descriptor to send commands to
 */
void parse_and_forward_commands(int in_fd, int out_fd);

/**
 * @brief Drives the robot in a square pattern
 *
 * @param fd File descriptor to send commands to the robot
 */
void drive_square(int fd, char start_dir, float speed, int delay_ms);

/**
 * @brief Converts WASDQE chars read from the input file descriptor into drive
 *        commands and sends them to the output file descriptor
 *
 * @param in_fd File descriptor to read commands from
 * @param out_fd File descriptor to send commands to the robot
 */
void convert_keys_to_drive_commands(int in_fd, int out_fd);

#ifdef __cplusplus
}
#endif