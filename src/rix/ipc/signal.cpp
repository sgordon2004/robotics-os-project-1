#include "rix/ipc/signal.hpp"

#include <stdexcept>
#include <iostream>

namespace rix {
namespace ipc {

std::array<Signal::SignalNotifier, 32> Signal::notifier = {};

// Constructs a new Signal object.
// Throws a `std::invalid_argument` error if `signum` is less than 1 or greater than 32
// or if another Signal object with the same value exists already.
Signal::Signal(int signum) : signum_(signum - 1) {
    // Confirm signal number is between 1 and 32
    if (signum < 1 || signum > 32) {
        throw std::invalid_argument("Signal number must be between 1 and 32.");
    }

    // Check if a Signal object with this signal number already exists
    if (notifier[signum - 1].is_init) {
        throw std::invalid_argument("Signal object with this signal number already exists.");
    }

    // Create the pipe pair for this signal (0: read-end, 1: write-end)
    notifier[signum - 1].pipe = Pipe::create();
    notifier[signum - 1].is_init = true;

    // Register the static signal handler
    ::signal(signum, Signal::handler);
}

// Destroy the signal object.
// If the signal is in a valid state, resets the signal to the default behavior.
Signal::~Signal() {
    if (signum_ >= 0) { // Only if in a valid state
        // Reset signal to default behavior
        ::signal(signum_ + 1, SIG_DFL);

        // Mark this signal notifier as uninitialized (allows another signal to be created with this signal number)
        notifier[signum_].is_init = false;
    }
}

Signal::Signal(Signal &&other) : signum_(other.signum_) {
    other.signum_ = -1; // invalidates source
}

Signal &Signal::operator=(Signal &&other) {
    if (this != &other && other.signum_ >= 0) {
        if (signum_ >= 0) {
            ::signal(signum_ + 1, SIG_DFL);
            notifier[signum_].is_init = false;
        }
        signum_ = other.signum_;
        other.signum_ = -1; // invalidate source
    }
    return *this;
}

// Raise the signal in the current process.
// If the signal is invalid, return false.
// Return true if raise system call was successful.
bool Signal::raise() const { 
    if (signum_ < 0) { // if invalid
        return false;
    }
     // Raise the signal (convert signum_ back to 1-32)
     return ::raise(signum_ + 1) == 0;
}

// Send the signal to the process specified by `pid`.
// If the signal is invalid, return false.
// Return true if kill was successful.
bool Signal::kill(pid_t pid) const {
    if (signum_ < 0) { // if invalid
        return false;
    }

    // Send the signal to the specified process
    return ::kill(pid, signum_ + 1) == 0;
}

int Signal::signum() const {
    if (signum_ < 0) return -1; // return -1 if invalid
    return signum_ + 1; // return 1-32 for valid signals
}

// Wait until the signal is received, or until the specified duration elapses.
// If the signal is invalid, return false.
// Return true if the signal arrived within the specified time.
bool Signal::wait(const rix::util::Duration &d) const {
    if (signum_ < 0) { // if invalid
        return false;
    }

    // Wait for the signal to arrive by waiting for the pipe to become readable
    // The read-end is index 0 of the pipe array
    bool is_readable = notifier[signum_].pipe[0].wait_for_readable(d);

    // If the pipe is readable, consume the signal byte
    if (is_readable) {
        unsigned char byte;
        notifier[signum_].pipe[0].read(&byte, 1);
    }

    return is_readable;
}

// The signal handler. This must be a static function because the
// Standard C Library signal API requires the function to return void and
// have a single integer argument. If this were a member function, it would
// have an implicit Signal* argument. This implies that any data accessed
// or modified by the handler must also be global or static.
void Signal::handler(int signum) {
    // Convert signal number (1-32) to array index (0-31)
    int index = signum - 1;

    // Only proceed if the notifier for this signal has been initialized
    if (notifier[index].is_init) {
        // Write a single byte to the write-end (inddex 1) of the pipe
        // This signals that the signal has been received
        unsigned char byte = 1;
        ::write(notifier[index].pipe[1].fd(), &byte, 1);
    }
}

}  // namespace ipc
}  // namespace rix