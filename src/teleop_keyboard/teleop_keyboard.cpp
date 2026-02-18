#include <teleop_keyboard/teleop_keyboard.hpp>

TeleopKeyboard::TeleopKeyboard(std::unique_ptr<rix::ipc::interfaces::IO> input,
                               std::unique_ptr<rix::ipc::interfaces::IO> output, double linear_speed,
                               double angular_speed)
    : input(std::move(input)), output(std::move(output)), linear_speed(linear_speed), angular_speed(angular_speed) {}

void TeleopKeyboard::spin(std::unique_ptr<rix::ipc::interfaces::Notification> notif) {
    uint32_t seq = 0;
    uint8_t buffer[4096];

    while (true) {
        // Check SIGINT
        if (notif->is_ready()) {
            return;
        }

        // Read character from FIFO
        ssize_t bytes_read = input->read(buffer, 1);
        if (bytes_read != 1) {
            continue; // No data available or error
        }

        char ch = (char)buffer[0];

        // Map character to velocities
        double vx = 0, vy = 0, wz = 0;
        switch(ch) {
            case 'W': case 'w': vx = linear_speed; break;
            case 'A': case 'a': vy = linear_speed; break;
            case 'S': case 's': vx = -linear_speed; break;
            case 'D': case 'd': vy = -linear_speed; break;
            case 'Q': case 'q': wz = angular_speed; break;
            case 'E': case 'e': wz = -angular_speed; break;
            case ' ': break;
            default: continue; // ignore unknown keys
        }

        // Create and send Twist2DStamped
        geometry::Twist2DStamped cmd;
        cmd.header.seq = seq++;
        cmd.header.frame_id = "mbot";
        cmd.header.stamp = Time::now().to_msg();
        cmd.twist.vx = (float)vx;
        cmd.twist.vy = (float)vy;
        cmd.twist.wz = (float)wz;

        // Serialize message size and data
        uint8_t msg_buffer[4096];
        size_t offset = 0;

        // First serialize the size
        standard::UInt32 size_msg;
        size_msg.data = cmd.size();
        size_msg.serialize(msg_buffer, offset);

        // Then serialize the message
        cmd.serialize(msg_buffer, offset);

        // Write to stdout
        output->write(msg_buffer, offset);
    }
}