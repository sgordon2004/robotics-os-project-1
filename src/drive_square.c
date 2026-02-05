#include "robot/robot.h"
#include "util.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <use_serial> <start_dir> <speed> <delay_ms>\n", argv[0]);
        exit(1);
    }

    int use_serial, delay_ms;
    char start_dir;
    float speed;
    // TODO: Parse command line arguments.
    // Hint: Use `atoi` to convert a string to an integer
    // Hint: argv is an array of strings, where argv[0] is the program name
    //       and argv[1] is the first argument

    // TODO: Set up signal handler for SIGINT

    int fd[2];
    // TODO: Create pipe

    pid_t pid;
    // TODO: Fork process

    if (pid == 0) {
        // Child process

        // TODO: Close unused end of pipe

        // TODO: Initialize the robot

        // TODO: Relay drive commands from the pipe to the robot

        // TODO: Deinitialize the robot

        // TODO: Close file descriptor for the used end of the pipe

    } else {
        // Parent process

        // TODO: Close unused end of pipe

        // TODO: Send drive commands to the pipe

        // TODO: Close file descriptor for the used end of the pipe
        
    }

    return 0;
}
