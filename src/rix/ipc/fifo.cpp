#include "rix/ipc/fifo.hpp"

namespace rix {
namespace ipc {

// Creates a Fifo object by opening a named pipe specified by the pathname.
// This will create the fifo special file if it does not exist.
// The fifo special file will be opened for both reading and writing.
Fifo::Fifo(const std::string &pathname, Mode mode, bool nonblocking)
    : File(), mode_(mode), pathname_(pathname) {
    // Create the FIFO file if it does not exist
    int result = ::mkfifo(pathname.c_str(), 0666);

    // Determine open flags based on mode
    int flags;
    if (mode == Mode::READ) {
        flags = O_RDONLY; // Open for reading only
    } else {
        flags = O_WRONLY; // Open for writing only
    }

    // Add non-blocking flag if requested
    if (nonblocking) {
        flags |= O_NONBLOCK;
    }

    // Open the FIFO and store the file descriptor
    fd_ = ::open(pathname.c_str(), flags);
}

Fifo::Fifo() {}

// Copy constructor - duplicates underlying file descriptor using  `dup`
Fifo::Fifo(const Fifo &src) : File(), mode_(src.mode_), pathname_(src.pathname_) {
    if (src.fd_ >= 0 ) { // if the file is valid
        fd_ = ::dup(src.fd_);
    }
}


// Assignment operator - duplicates underlying file descriptor using  `dup`
Fifo &Fifo::operator=(const Fifo &src) {
    if (this != &src) { // avoids self-assignment
        if (fd_ > 0) { // if the file is valid
            ::close(fd_); // close existing fd
            fd_ = -1;
        }
        if (src.fd_ >= 0) { // if the source file descriptor is valid
            fd_ = ::dup(src.fd_);
        } else {
            fd_ = -1;
        }
        mode_ = src.mode_; // copy mode
        pathname_ = src.pathname_; // copy pathname
    }
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