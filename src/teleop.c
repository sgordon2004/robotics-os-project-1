#include "robot/robot.h"
#include "util.h"

int main() {
    // TODO: Set up signal handler for SIGINT

    // TODO: Create FIFO for receiving teleop commands. The name of this FIFO
    //       should be "/tmp/teleop_cmd" (Hint: use 0666 as the mode in mkfifo.
    //       This means that all users can read and write but cannot execute 
    //       the file/folder. 
    //       See chmod: https://man7.org/linux/man-pages/man2/chmod.2.html)

    // TODO: Open FIFO to get file descriptor for reading teleop commands

    // TODO: Convert keys to drive commands

    // TODO: Close the FIFO

    // TODO: Unlink the FIFO
    
}