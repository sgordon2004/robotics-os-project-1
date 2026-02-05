#include "robot/robot.h"
#include "util.h"

// Instantiate the `sigint_flag` variable
int sigint_flag = 0;

void signal_handler(int signum) {
    // TODO: Implement signal_handler function.
    //       If the signal is SIGINT, set the `sigint_flag` variable to 1
}

void relay_drive_commands(int in_fd) {
    // TODO: Implement relay_drive_commands function.
    //       Read drive commands from `in_fd` and send them to the robot using
    //       the drive function defined in `include/robot/robot.h`. Stop when a
    //       SIGINT signal is received.
}

ssize_t read_line(int fd, char *buffer, size_t size) {
    // TODO: Implement read_line function. Read characters from `fd` until a
    //       newline character is encountered, or `size` characters have been
    //       read. Store the characters in `buffer` and return the number of
    //       characters read. If an error occurs, return -1.

    return -1;
}

void parse_and_forward_commands(int in_fd, int out_fd) {
    // TODO: Implement parse_and_forward_commands function. Read a line from 
    //       `in_fd` and parse it as a drive command (Hint: see sscanf). Send 
    //       the drive command to `out_fd` and then sleep for the specified
    //       delay. Stop when a SIGINT signal is received. If an error occurs or
    //       if the command is not in the expected format, return.
    // The format of the input line is:
    // <id: string> <vx: float> <vy: float> <wz: float> <delay: int>
}

void drive_square(int fd, char start_dir, float speed, int delay_ms) {
    // TODO: Implement drive_square function. Send drive commands to the robot to
    //       drive in a square pattern going clock-wise starting in the start direction.
    //       The robot should drive at the specified speed and sleep for the 
    //       specified delay after each command.
    //       Stop when a SIGINT signal is received.
    // Hint: If the start direction is 'N', the robot should drive
    //       North, East, South, West, and then stop.
    // Hint: Consider using a state machine to implement this function.
}

void convert_keys_to_drive_commands(int in_fd, int out_fd) {
    // TODO: Implement convert_keys_to_drive_commands function. Read characters
    //       from `in_fd` and convert them to drive commands. Send the drive
    //       commands to `out_fd`. Stop when a SIGINT signal is received.
    //       Your linear speed should be 0.5 m/s and your angular speed should
    //       be 1.5 rad/s. 
    // The keys to drive the robot are:
    //       - 'w': forward
    //       - 'a': left
    //       - 's': backward
    //       - 'd': right
    //       - 'q': rotate left
    //       - 'e': rotate right
    //       - ' ': stop
}