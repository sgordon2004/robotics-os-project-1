#include "mbot_driver/mbot_driver.hpp"

using namespace rix::ipc;
using namespace rix::msg;

MBotDriver::MBotDriver(std::unique_ptr<interfaces::IO> input, std::unique_ptr<MBotBase> mbot)
    : input(std::move(input)), mbot(std::move(mbot)) {}

void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
    // Allocate buffer for reading messages (max message size)
    uint8_t buffer[4096];

    while (true) {
        // Check if SIGINT received
        if (notif->is_ready()) {
            // Send stop command before exiting
            geometry::Twist2DStamped stop_cmd;
            stop_cmd.twist.vx = 0.0;
            stop_cmd.twist.vy = 0.0;
            stop_cmd.twist.wz = 0.0;
            mbot->drive(stop_cmd);
            return;
        }

        // Read 4-byte message size
        ssize_t size_bytes_read = input->read(buffer, 4);

        // Check for EOF (0 bytes read)
        if (size_bytes_read == 0) {
            // Send stop command before exiting
            geometry::Twist2DStamped stop_cmd;
            stop_cmd.twist.vx = 0.0;
            stop_cmd.twist.vy = 0.0;
            stop_cmd.twist.wz = 0.0;
            mbot->drive(stop_cmd);
            return;
        }

        // Error reading size
        if (size_bytes_read != 4) {
            continue;
        }

        // Deserialize size
        size_t offset = 0;
        standard::UInt32 size_msg;
        if (!size_msg.deserialize(buffer, 4, offset)) {
            continue;
        }
        uint32_t msg_size = size_msg.data;

        // Read message data
        ssize_t msg_bytes_read = input->read(buffer, msg_size);
        if (msg_bytes_read != (ssize_t)msg_size) {
            continue;
        }

        // Deserialize Twist2DStamped
        offset = 0;
        geometry::Twist2DStamped cmd;
        if (!cmd.deserialize(buffer, msg_size, offset)) {
            continue;
        }

        // Send command to Mbot
        mbot->drive(cmd);
    }
}