#pragma once

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Drive command structure
 */
#pragma pack(push, 1)
typedef struct drive_cmd_t {
    int64_t utime;
    float vx;
    float vy;
    float wz;
} drive_cmd_t;
#pragma pack(pop)

/**
 * @brief Opens and configures the serial port for the robot and starts the time synchronization thread
 *
 * @return 0 on success, -1 on error
 */
int init_mbot(int use_serial);

/**
 * @brief Stops the time synchronization thread and closes the serial port for the robot
*/
void deinit_mbot();

/**
 * @brief Sends a drive command to the robot
 *
 * @param cmd Drive command to send
 */
void drive(const drive_cmd_t *cmd);

#ifdef __cplusplus
}
#endif