#include "rix/ipc/signal.hpp"

#include <iostream>

namespace rix {
namespace ipc {

std::array<Signal::SignalNotifier, 32> Signal::notifier = {};

/**< TODO */
Signal::Signal(int signum) : signum_(signum - 1) {}

/**< TODO */
Signal::~Signal() {}

Signal::Signal(Signal &&other) : signum_(-1) { std::swap(signum_, other.signum_); }

Signal &Signal::operator=(Signal &&other) {
    if (signum_ >= 0 && signum_ < 32) {
        if (notifier[signum_].is_init) {
            notifier[signum_].is_init = false;
            notifier[signum_].pipe = {};
            ::signal(signum_, SIG_DFL);
        }
    }
    signum_ = -1;
    std::swap(signum_, other.signum_);
    return *this;
}

/**< TODO */
bool Signal::raise() const { 
    return false; 
}

/**< TODO */
bool Signal::kill(pid_t pid) const {
    return false;
}

int Signal::signum() const { return signum_ + 1; }

/**< TODO */
bool Signal::wait(const rix::util::Duration &d) const {
    return false;
}

/**< TODO */
void Signal::handler(int signum) {}

}  // namespace ipc
}  // namespace rix