#include <pthread.h>
#include <stdatomic.h>

#include "robot/robot.h"

/// PRIVATE DECLARATIONS ///

#define SYNC_FLAG 0xff                                         /**< Sync flag for ROS packets */
#define VERSION_FLAG 0xfe                                      /**< Version flag for ROS packets */
#define ROS_HEADER_LENGTH 7                                    /**< Length of the ROS packet header */
#define ROS_FOOTER_LENGTH 1                                    /**< Length of the ROS packet footer */
#define ROS_PKG_LENGTH (ROS_HEADER_LENGTH + ROS_FOOTER_LENGTH) /**< Length of the ROS packet overhead */

#define MBOT_VEL_CMD 214  /**< Topic ID for velocity commands */
#define MBOT_TIMESYNC 201 /**< Topic ID for time synchronization */

/**
 * @brief Message structure for time synchronization
 */
#pragma pack(push, 1)
typedef struct timesync_msg_t {
    int64_t utime;
} timesync_msg_t;
#pragma pack(pop)

/**
 * @brief Evaluates the checksum of an array of bytes
 *
 * @param addends Array of bytes to checksum
 * @param len Length of the array
 */
uint8_t checksum(uint8_t *addends, int len);

/**
 * @brief Encodes a message into a ROS packet defined by the [ROS serial
 * protocol](http://wiki.ros.org/rosserial/Overview/Protocol)
 *
 * @param msg Message to encode
 * @param msg_len Length of the message
 * @param topic Topic ID for the message
 * @param rospkt Buffer to store the ROS packet
 * @param rospkt_len Length of the ROS packet buffer
 */
int encode_msg(uint8_t *msg, int msg_len, uint16_t topic, uint8_t *rospkt, int rospkt_len);

/**
 * @brief Thread function for time synchronization
 *
 * @param arg File descriptor for the robot
 */
void *timesync(void *arg);

pthread_mutex_t robot_fd_mutex;     /**< Mutex for the robot file descriptor */
atomic_bool stop_timesync_flag = 0; /**< Flag to stop the time synchronization thread */
pthread_t timesync_thread;          /**< Thread for time synchronization */
int robot_fd;                       /**< File descriptor for the robot */
int use_serial_flag;                /**< Flag to use serial port */

/// PUBLIC FUNCTION IMPLEMENTATIONS ///

int init_mbot(int use_serial) {
    use_serial_flag = use_serial;
    // open the file descriptor for the robot
    if (use_serial) {
        robot_fd = open("/dev/mbot_lcm", O_RDWR | O_NOCTTY | O_NDELAY);
        if (robot_fd < 0) {
            perror("open");
            return -1;
        }
        // Set up the serial port
        struct termios options;
        tcgetattr(robot_fd, &options);
        cfsetspeed(&options, B115200);
        options.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CRTSCTS);
        options.c_cflag |= CS8 | CREAD | CLOCAL;
        options.c_oflag &= ~OPOST;
        options.c_lflag &= ~(ICANON | ISIG | ECHO | IEXTEN); /* Set non-canonical mode */
        options.c_cc[VTIME] = 1;
        options.c_cc[VMIN] = 0;
        cfmakeraw(&options);
        tcflush(robot_fd, TCIFLUSH);
        tcsetattr(robot_fd, TCSANOW, &options);
        if (tcgetattr(robot_fd, &options) != 0) {
            close(robot_fd);
            return -1;
        }
    } else {
        robot_fd = STDOUT_FILENO;
    }

    // Initialize the mutex
    pthread_mutex_init(&robot_fd_mutex, NULL);

    // Start the time synchronization thread
    if (use_serial) {
        pthread_create(&timesync_thread, NULL, timesync, NULL);
    }

    return 0;
}

void deinit_mbot() {
    // Set the stop flag for the time synchronization thread
    stop_timesync_flag = 1;

    // Join the time synchronization thread
    if (use_serial_flag) {
        pthread_join(timesync_thread, NULL);
    }

    // Close the file descriptor for the robot
    close(robot_fd);

    // Destroy the mutex
    pthread_mutex_destroy(&robot_fd_mutex);
}

void drive(const drive_cmd_t *cmd) {
    // Encode the drive command
    uint8_t msg[sizeof(drive_cmd_t) + ROS_PKG_LENGTH];
    encode_msg((uint8_t *)cmd, sizeof(drive_cmd_t), MBOT_VEL_CMD, msg, sizeof(msg));

    // Send the drive command
    pthread_mutex_lock(&robot_fd_mutex);
    write(robot_fd, msg, sizeof(msg));
    pthread_mutex_unlock(&robot_fd_mutex);
}

/// PRIVATE FUNCTION IMPLEMENTATIONS ///

uint8_t checksum(uint8_t *addends, int len) {
    // takes in an array and sums the contents then checksums the array
    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += addends[i];
    }
    return 255 - ((sum) % 256);
}

int encode_msg(uint8_t *msg, int msg_len, uint16_t topic, uint8_t *rospkt, int rospkt_len) {
    // SANITY CHECKS
    if (msg_len + ROS_PKG_LENGTH != rospkt_len) {
        return -1;
    }

    // CREATE ROS PACKET
    // for ROS protocol and packet format see link: http://wiki.ros.org/rosserial/Overview/Protocol
    rospkt[0] = SYNC_FLAG;
    rospkt[1] = VERSION_FLAG;
    rospkt[2] = (uint8_t)(msg_len & 0xFF);  // message length lower 8/16b via bitwise AND and cast
    rospkt[3] = (uint8_t)(msg_len >> 8);    // message length higher 8/16b via bitshift and cast

    uint8_t cs1_addends[2] = {rospkt[2], rospkt[3]};
    rospkt[4] = checksum(cs1_addends, 2);  // checksum over message length
    rospkt[5] = (uint8_t)(topic & 0xFF);   // message topic lower 8/16b via bitwise AND and cast
    rospkt[6] = (uint8_t)(topic >> 8);     // message length higher 8/16b via bitshift and cast

    memcpy(&rospkt[ROS_HEADER_LENGTH], msg, msg_len);  // copy message data to packet

    uint8_t cs2_addends[msg_len + 2];  // create array for the checksum over topic and message content
    cs2_addends[0] = rospkt[5];
    cs2_addends[1] = rospkt[6];
    for (int i = 0; i < msg_len; i++) {
        cs2_addends[i + 2] = msg[i];
    }

    rospkt[rospkt_len - 1] = checksum(cs2_addends, msg_len + 2);  // checksum over message data and topic

    return 0;
}

void *timesync(void *arg) {
    int status;

    // Time synchronization loop
    while (!stop_timesync_flag) {
        // Encode the timesync message
        timesync_msg_t msg = {0};
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        msg.utime = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
        const size_t msg_size = sizeof(timesync_msg_t) + ROS_PKG_LENGTH;
        uint8_t rospkt[msg_size];
        if (encode_msg((uint8_t *)&msg, sizeof(timesync_msg_t), MBOT_TIMESYNC, rospkt, msg_size) < 0) {
            perror("encode_msg");
            break;
        }

        // Send the timesync message
        pthread_mutex_lock(&robot_fd_mutex);
        status = write(robot_fd, rospkt, msg_size);
        pthread_mutex_unlock(&robot_fd_mutex);
        if (status < 0) {
            perror("write");
            break;
        }

        // Run at 2 Hz
        usleep(500000);
    }
    return NULL;
}