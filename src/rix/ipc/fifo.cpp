#include "rix/ipc/fifo.hpp"

namespace rix {
namespace ipc {

/**< TODO */
Fifo::Fifo(const std::string &pathname, Mode mode, bool nonblocking) {}

Fifo::Fifo() {}

/**< TODO */
Fifo::Fifo(const Fifo &other) {}

/**< TODO */
Fifo &Fifo::operator=(const Fifo &other) {
    return *this;
}

Fifo::Fifo(Fifo &&other)
    : File(std::move(other)),
      pathname_(std::move(other.pathname_)),
      mode_(std::move(other.mode_)) {
}

Fifo &Fifo::operator=(Fifo &&other) {
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
        pathname_ = "";
    }
    std::swap(fd_, other.fd_);
    pathname_ = std::move(other.pathname_);
    mode_ = std::move(other.mode_);
    return *this;
}

Fifo::~Fifo() {}

std::string Fifo::pathname() const { return pathname_; }

Fifo::Mode Fifo::mode() const { return mode_; }

}  // namespace ipc
}  // namespace rix