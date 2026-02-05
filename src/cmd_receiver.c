#include "robot/robot.h"
#include "util.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <use_serial>\n", argv[0]);
        exit(1);
    }

    // TODO: Parse command line arguments.
    // Hint: Use `atoi` to convert a string to an integer
    // Hint: argv is an array of strings, where argv[0] is the program name
    //       and argv[1] is the first argument

    // TODO: Set up signal handler for SIGINT

    // TODO: Initialize the robot. Use the parsed argument to determine if 
    //       serial should be used

    // TODO: Relay drive commands from STDIN to the robot

    // TODO: Deinitialize the robot

    return 0;
}
